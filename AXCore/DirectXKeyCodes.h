
/* ------------------------------------
	Scan codes copied from dxinput
------------------------------------ */

#define AX_MOUSE0 0
#define AX_MOUSE1 1
#define AX_MOUSE2 2
#define AX_MOUSE3 3
#define AX_MOUSE4 4
#define AX_MOUSE5 5
#define AX_MOUSE6 6
#define AX_MOUSE7 7

#define AX_ESCAPE          0x01
#define AX_1               0x02
#define AX_2               0x03
#define AX_3               0x04
#define AX_4               0x05
#define AX_5               0x06
#define AX_6               0x07
#define AX_7               0x08
#define AX_8               0x09
#define AX_9               0x0A
#define AX_0               0x0B
#define AX_MINUS           0x0C    /* - on main keyboard */
#define AX_EQUALS          0x0D
#define AX_BACK            0x0E    /* backspace */
#define AX_TAB             0x0F
#define AX_Q               0x10
#define AX_W               0x11
#define AX_E               0x12
#define AX_R               0x13
#define AX_T               0x14
#define AX_Y               0x15
#define AX_U               0x16
#define AX_I               0x17
#define AX_O               0x18
#define AX_P               0x19
#define AX_LBRACKET        0x1A
#define AX_RBRACKET        0x1B
#define AX_RETURN          0x1C    /* Enter on main keyboard */
#define AX_LCONTROL        0x1D
#define AX_A               0x1E
#define AX_S               0x1F
#define AX_D               0x20
#define AX_F               0x21
#define AX_G               0x22
#define AX_H               0x23
#define AX_J               0x24
#define AX_K               0x25
#define AX_L               0x26
#define AX_SEMICOLON       0x27
#define AX_APOSTROPHE      0x28
#define AX_GRAVE           0x29    /* accent grave */
#define AX_LSHIFT          0x2A
#define AX_BACKSLASH       0x2B
#define AX_Z               0x2C
#define AX_X               0x2D
#define AX_C               0x2E
#define AX_V               0x2F
#define AX_B               0x30
#define AX_N               0x31
#define AX_M               0x32
#define AX_COMMA           0x33
#define AX_PERIOD          0x34    /* . on main keyboard */
#define AX_SLASH           0x35    /* / on main keyboard */
#define AX_RSHIFT          0x36
#define AX_MULTIPLY        0x37    /* * on numeric keypad */
#define AX_LMENU           0x38    /* left Alt */
#define AX_SPACE           0x39
#define AX_CAPITAL         0x3A
#define AX_F1              0x3B
#define AX_F2              0x3C
#define AX_F3              0x3D
#define AX_F4              0x3E
#define AX_F5              0x3F
#define AX_F6              0x40
#define AX_F7              0x41
#define AX_F8              0x42
#define AX_F9              0x43
#define AX_F10             0x44
#define AX_NUMLOCK         0x45
#define AX_SCROLL          0x46    /* Scroll Lock */
#define AX_NUMPAD7         0x47
#define AX_NUMPAD8         0x48
#define AX_NUMPAD9         0x49
#define AX_SUBTRACT        0x4A    /* - on numeric keypad */
#define AX_NUMPAD4         0x4B
#define AX_NUMPAD5         0x4C
#define AX_NUMPAD6         0x4D
#define AX_ADD             0x4E    /* + on numeric keypad */
#define AX_NUMPAD1         0x4F
#define AX_NUMPAD2         0x50
#define AX_NUMPAD3         0x51
#define AX_NUMPAD0         0x52
#define AX_DECIMAL         0x53    /* . on numeric keypad */
#define AX_OEM_102         0x56    /* <> or \| on RT 102-key keyboard (Non-U.S.) */
#define AX_F11             0x57
#define AX_F12             0x58
#define AX_F13             0x64    /*                     (NEC PC98) */
#define AX_F14             0x65    /*                     (NEC PC98) */
#define AX_F15             0x66    /*                     (NEC PC98) */
#define AX_KANA            0x70    /* (Japanese keyboard)            */
#define AX_ABNT_C1         0x73    /* /? on Brazilian keyboard */
#define AX_CONVERT         0x79    /* (Japanese keyboard)            */
#define AX_NOCONVERT       0x7B    /* (Japanese keyboard)            */
#define AX_YEN             0x7D    /* (Japanese keyboard)            */
#define AX_ABNT_C2         0x7E    /* Numpad . on Brazilian keyboard */
#define AX_NUMPADEQUALS    0x8D    /* = on numeric keypad (NEC PC98) */
#define AX_PREVTRACK       0x90    /* Previous Track (AX_CIRCUMFLEX on Japanese keyboard) */
#define AX_AT              0x91    /*                     (NEC PC98) */
#define AX_COLON           0x92    /*                     (NEC PC98) */
#define AX_UNDERLINE       0x93    /*                     (NEC PC98) */
#define AX_KANJI           0x94    /* (Japanese keyboard)            */
#define AX_STOP            0x95    /*                     (NEC PC98) */
#define AX_AX              0x96    /*                     (Japan AX) */
#define AX_UNLABELED       0x97    /*                        (J3100) */
#define AX_NEXTTRACK       0x99    /* Next Track */
#define AX_NUMPADENTER     0x9C    /* Enter on numeric keypad */
#define AX_RCONTROL        0x9D
#define AX_MUTE            0xA0    /* Mute */
#define AX_CALCULATOR      0xA1    /* Calculator */
#define AX_PLAYPAUSE       0xA2    /* Play / Pause */
#define AX_MEDIASTOP       0xA4    /* Media Stop */
#define AX_VOLUMEDOWN      0xAE    /* Volume - */
#define AX_VOLUMEUP        0xB0    /* Volume + */
#define AX_WEBHOME         0xB2    /* Web home */
#define AX_NUMPADCOMMA     0xB3    /* , on numeric keypad (NEC PC98) */
#define AX_DIVIDE          0xB5    /* / on numeric keypad */
#define AX_SYSRQ           0xB7
#define AX_RMENU           0xB8    /* right Alt */
#define AX_PAUSE           0xC5    /* Pause */
#define AX_HOME            0xC7    /* Home on arrow keypad */
#define AX_UP              0xC8    /* UpArrow on arrow keypad */
#define AX_PRIOR           0xC9    /* PgUp on arrow keypad */
#define AX_LEFT            0xCB    /* LeftArrow on arrow keypad */
#define AX_RIGHT           0xCD    /* RightArrow on arrow keypad */
#define AX_END             0xCF    /* End on arrow keypad */
#define AX_DOWN            0xD0    /* DownArrow on arrow keypad */
#define AX_NEXT            0xD1    /* PgDn on arrow keypad */
#define AX_INSERT          0xD2    /* Insert on arrow keypad */
#define AX_DELETE          0xD3    /* Delete on arrow keypad */
#define AX_LWIN            0xDB    /* Left Windows key */
#define AX_RWIN            0xDC    /* Right Windows key */
#define AX_APPS            0xDD    /* AppMenu key */
#define AX_POWER           0xDE    /* System Power */
#define AX_SLEEP           0xDF    /* System Sleep */
#define AX_WAKE            0xE3    /* System Wake */
#define AX_WEBSEARCH       0xE5    /* Web Search */
#define AX_WEBFAVORITES    0xE6    /* Web Favorites */
#define AX_WEBREFRESH      0xE7    /* Web Refresh */
#define AX_WEBSTOP         0xE8    /* Web Stop */
#define AX_WEBFORWARD      0xE9    /* Web Forward */
#define AX_WEBBACK         0xEA    /* Web Back */
#define AX_MYCOMPUTER      0xEB    /* My Computer */
#define AX_MAIL            0xEC    /* Mail */
#define AX_MEDIASELECT     0xED    /* Media Select */

/*
 *  Alternate names for keys, to facilitate transition from DOS.
 */
#define AX_BACKSPACE       AX_BACK            /* backspace */
#define AX_NUMPADSTAR      AX_MULTIPLY        /* * on numeric keypad */
#define AX_LALT            AX_LMENU           /* left Alt */
#define AX_CAPSLOCK        AX_CAPITAL         /* CapsLock */
#define AX_NUMPADMINUS     AX_SUBTRACT        /* - on numeric keypad */
#define AX_NUMPADPLUS      AX_ADD             /* + on numeric keypad */
#define AX_NUMPADPERIOD    AX_DECIMAL         /* . on numeric keypad */
#define AX_NUMPADSLASH     AX_DIVIDE          /* / on numeric keypad */
#define AX_RALT            AX_RMENU           /* right Alt */
#define AX_UPARROW         AX_UP              /* UpArrow on arrow keypad */
#define AX_PGUP            AX_PRIOR           /* PgUp on arrow keypad */
#define AX_LEFTARROW       AX_LEFT            /* LeftArrow on arrow keypad */
#define AX_RIGHTARROW      AX_RIGHT           /* RightArrow on arrow keypad */
#define AX_DOWNARROW       AX_DOWN            /* DownArrow on arrow keypad */
#define AX_PGDN            AX_NEXT            /* PgDn on arrow keypad */