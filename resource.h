#define IDS_PROJNAME					100
#define IDD_COMMENT_PROPPAGE			107
#define IDI_ICON						201
#define IDI_TAB_ICON					201
#define IDC_TEXT						202
#define IDC_TABCTRL						203
#define IDD_COMMENT						204
#define IDD_SETTINGS					205

#define IDD_COMMENT_TAB					210
#define IDD_SETTINGS_TAB				211

// Settings tab controls
#define IDC_STATIC_CP					231
#define IDC_COMBO_CP					232
#define IDC_STATIC_NL					233
//WARNING: Checkboxes and ratio buttons should be in sequential order so that
// if (LOWORD(wParam) >= IDC_CHK_BOM && LOWORD(wParam) <= IDC_RADIO_NL_NONE)
// in property_sheet.cpp works alright.
#define IDC_CHK_BOM						234
#define IDC_RADIO_NL_AUTO				235
#define IDC_RADIO_NL_DBL				236
#define IDC_RADIO_NL_TC					237
#define IDC_RADIO_NL_NONE				238
#define IDC_CHK_CLEAN					239
#define IDC_STATIC_STATUS				240
