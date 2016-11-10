/**
  * Copyright (c) 2016 Gavriel Fleischer <flocsy@gmail.com>
  *
  * Use `npm install pebble-clay --save` to add pebble-clay to your app.
  * See: app.js
  * This is a simple configuration with 1 toggle to enable/disable RTLTR.
  *
  * IMPORTANT: You need to keep the `"messageKey": "RTLTR_OVERRIDE"` to work. All the rest can be changed.
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
        "defaultValue": "Appearance"
      },
      {
        "type": "toggle",
        "messageKey": "SHOW_SECONDS",
        "label": "Show seconds hand",
        "defaultValue": true
      }
    ]
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
        "defaultValue": "Override RTLTR if Hebrew texts are displayed in the incorrect direction."
      },
      {
        "type": "toggle",
        "messageKey": "RTLTR_OVERRIDE",
        "label": "Override RTLTR",
        "defaultValue": false
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];
