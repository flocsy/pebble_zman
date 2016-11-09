/**
  * Copyright (c) 2016 Gavriel Fleischer <flocsy@gmail.com>
  *
  * Use `npm install pebble-clay --save` to add pebble-clay to your app.
  * See: app.js
  * This is a simple configuration with 1 toggle to enable/disable RTLTR.
  *
  * IMPORTANT: You need to keep the `"messageKey": "ENABLE_RTLTR"` to work. All the rest can be changed.
  */

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
