typedef struct {
  const char* str;
  uint32_t wait;
} Step;

typedef struct {
  int repeat;
  int len;

  int active;
  int index;
  int waiting;
  uint32_t timer;

  Step steps[];
} Macro;

typedef struct {
  int active;
  int waiting;
  int repeating;
  keypos_t keypos;
  uint32_t timer;

  int delay;
  const char* str;
} HoldRepeat;

void init_hold_repeat(HoldRepeat* h) {
  h->active = 0;
}

void start_hold_repeat(HoldRepeat* h, keyrecord_t *record) {
  h->active = 1;
  h->waiting = 0;
  h->repeating = 0;
  h->keypos = record->event.key;
}

void release_hold_repeat(HoldRepeat* h) {
  if (!h->repeating) {
    h->active = 0;
  }
}

void step_hold_repeat(HoldRepeat* h) {
  if (!h->active) {
    return;
  }
  if (h->waiting) {
    // Check if we are done waiting.
    if (timer_elapsed32(h->timer) <= h->delay) {
      printf("... waiting\n");
      return;
    }
    h->repeating = 1;
  }

  send_string(h->str);

  h->timer = timer_read32();
  h->waiting = 1;
}

void init_twist(Macro* m, uint32_t wait) {
  m->active = 0;
  m->repeat = 1;

  m->len = 0;
  while (m->steps[m->len].str) {
    m->steps[m->len].wait = wait;
    m->len++;
  }
}

void init_macro(Macro* m) {
  m->active = 0;
  m->repeat = 0;

  m->len = 0;
  while (m->steps[m->len].str) {
    if (m->steps[m->len].wait < TAP_CODE_DELAY) {
      m->steps[m->len].wait = TAP_CODE_DELAY;
    }
    m->len++;
  }
}

void start_macro(Macro* m) {
  m->active = 1;
  m->index = 0;
  m->waiting = 0;
  m->timer = 0;
}

void start_macro_at(Macro* m, Macro* at) {
  m->active = 1;
  m->index = MIN(at->index, m->len - 1);
  m->waiting = at->waiting;
  m->timer = at->timer;
}

void delay_twist_step(Macro* m) {
  if (m->active && m->waiting) {
    m->timer = timer_read32();
  }
}

void end_macro(Macro* m) {
  m->active = 0;
}

void step_macro(Macro* m) {
  if (m->active) {
    const Step* step = &m->steps[m->index];
    if (m->waiting) {
      // Check if we are done waiting.
      if (timer_elapsed32(m->timer) <= step->wait) {
        //printf("... waiting\n");
        return;
      }

      m->waiting = 0;

      // Advance to the next step
      m->index++;
      // At the end?
      if (m->index >= m->len) {
        if (!m->repeat) {
          // Macro is done.
          end_macro(m);
          return;
        }

        // Repeat wraps around to the beginning.
        m->index = 0;
      }
      step = &m->steps[m->index];
    }

    send_string(step->str);

    if (!m->repeat && m->index + 1 >= m->len) {
      // At end and no repeat.
      end_macro(m);
      return;
    }

    m->timer = timer_read32();
    m->waiting = 1;
  }
}


static int song_delay = 3500;

static Macro *m_twist;
static keypos_t twist_keypos;
static Macro *m_pop;


#ifdef TESTING
#  define HB(k,f) { #k }
#else
#  define HB(k,f) { SS_TAP(X_F##f) }
#endif

const char* hot_bar_2 = SS_RSFT("2");

static Macro m_twist2 = { .steps = { // 2 song
  HB(3,5), HB(4,6),  {}
}};
static Macro m_twist3 = { .steps = { // 3 song
  HB(3,5), HB(4,6), HB(5,7), {}
}};
static Macro m_twist4 = { .steps = { // 4 song
  HB(3,5), HB(4,6), HB(5,7), HB(6,8), {}
}};
static Macro m_twist_mana = { .steps = { // mana pump
  HB(3,5), HB(4,6), HB(4,6), HB(4,6), {}
}};
static Macro m_pop5 = { .steps = { HB(5,7), {} }};
static Macro m_pop6 = { .steps = { HB(6,8), {} }};
static Macro m_pop7 = { .steps = { HB(7,11), {} }};
static Macro m_pop8 = { .steps = { HB(8,12), {} }};

static HoldRepeat m_h2 = {
  .delay = 750,
  .str = "2"
};

static HoldRepeat m_h3 = {
  .delay = 2400,
  .str = "3"
};

static Macro m_auto2 = { .steps = {
  { "2" },
  {}
}};

static Macro m_charm_break_root = { .steps = {
  { SS_TAP(X_F1) },
  { "3" },
  { "`" },
  {}
}};

static Macro training1 = { .repeat = 1, .steps = {
  { "3" },
  { "4" },
  { "5" },
  { "6" },
  {}
}};

static Macro training2 = { .repeat = 1, .steps = {
  { "3" },
  { SS_TAP(X_BTN1) },
  { "4" },
  { SS_TAP(X_BTN1) },
  { "5" },
  { SS_TAP(X_BTN1) },
  { "6" },
  { SS_TAP(X_BTN1) },
  {}
}};

void macro_post_init_user(void) {
  init_twist(&m_twist2, song_delay);
  init_twist(&m_twist3, song_delay);
  init_twist(&m_twist4, song_delay);
  init_twist(&m_twist_mana, song_delay);

  init_hold_repeat(&m_h2);
  init_hold_repeat(&m_h3);
  init_twist(&m_auto2, 500);

  init_macro(&m_pop5);
  init_macro(&m_pop6);
  init_macro(&m_pop7);
  init_macro(&m_pop8);

  init_macro(&m_charm_break_root);
  init_twist(&training1, 250);
  init_twist(&training2, 500);

}

void start_twist(Macro* new_twist, keyrecord_t *record, bool switch_hot_bar) {
  if (m_twist == new_twist)
    return;

  if (m_twist) {
    start_macro_at(new_twist, m_twist);
    end_macro(m_twist);
  } else {
    if (switch_hot_bar && hot_bar_2) {
      // Switch to hot bar 2
      send_string(hot_bar_2);
    }
    start_macro(new_twist);
  }
  m_twist = new_twist;
  twist_keypos = record->event.key;
}

void start_pop(Macro* new_pop) {
  if (m_twist) {
    // Resume the twist after this pop song.
    delay_twist_step(m_twist);
  }

  start_macro(new_pop);
  m_pop = new_pop;
}

void stop_twist(void) {
  if (m_twist) {
    end_macro(m_twist);
    m_twist = 0;
  }
}


