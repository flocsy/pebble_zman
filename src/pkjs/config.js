module.exports = [
  {
    "type": "heading",
    "defaultValue": "App Configuration"
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Language Settings"
      },
      {
        "type": "text",
        "defaultValue": "RTLTR: If you use the app/watchface on a RTL enabled firmware (like https://elbbeP.cpfx.ca/) enable RTLTR to display the Hebrew/Arabic texts in the correct direction."
      },
      {
        "type": "toggle",
        "messageKey": "ENABLE_RTLTR",
        "label": "Enable RTLTR",
        "defaultValue": false
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];
