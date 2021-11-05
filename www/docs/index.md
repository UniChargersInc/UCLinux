# United Chargers APIs

[![NPM](https://img.shields.io/npm/v/docsify-tabs.svg?style=flat-square)](https://www.npmjs.com/package/docsify-tabs)
[![Codacy grade](https://img.shields.io/codacy/grade/88939149488a4ab69e0b63f256d2c22f.svg?style=flat-square)](https://www.codacy.com/app/jhildenbiddle/docsify-tabs?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=jhildenbiddle/docsify-tabs&amp;utm_campaign=Badge_Grade)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square)](https://github.com/jhildenbiddle/docsify-tabs/blob/master/LICENSE)
[![Tweet](https://img.shields.io/twitter/url/http/shields.io.svg?style=social)](https://twitter.com/intent/tweet?url=https%3A%2F%2Fgithub.com%2Fjhildenbiddle%2Fdocsify-tabs&hashtags=docsify,developers,frontend,plugin)
<a class="github-button" href="https://github.com/jhildenbiddle/docsify-tabs" data-icon="octicon-star" data-show-count="true" aria-label="Star jhildenbiddle/docsify-tabs on GitHub">Star</a>

A [docsify.js](https://docsify.js.org) plugin for rendering tabbed content from markdown.

## Intruduction

v5.1.2

United Chargers is an AI-based optimization engine accessible through an API for charging point operators (CPO), car manufacturers, and developers. It delivers a toolset to generate optimized charging profiles for electric vehicle (EV) charging stations (Smart Charging).

The API was developed to implement dynamic Smart Charging in new or already deployed charging networks/locations. A central system that integrated United Chargers can trigger optimization requests and receives charging commands in the API responses. When United Chargers re-optimizes (for example when other vehicles arrived), you central system automatically receives an updated.

For each optimization request, United Chargers generates an individual load profile (charging profile in OCPP format) with the selected optimization method. The central system receives an optimized load profile (time series or single limits) as a response and forwards the command to the respective charging station.

The United Chargers dashboard visualizes all activities and related data.

If you have recommendations or suggestions, please send us a message to contact@unitedchargers.com.

## Optimization Options

United Chargers optimizes the charging events of connected EVs using, for example, the following data point: the charging volume, the expected departure time, the available grid capacity, the type of energy resources, and the costs of energy.

The optimization methods are based on various Machine Learning methods and enable quick access to the right techniques for implementing managed charging.

Currently, these types of optimization methods are available:

Peak Optimizer: Reduces the impact of EV charging on the grid behind or in-front the meter (electric bill). This method targets the goal to reduce peak loads in a charging network to avoid new investments in grid infrastructure (substations, transformers, or grid lines), to avoid peak demand charges to the utility (behind the meter) or to reduce grid-losses.

Renewable Optimizer: Increases the usage of renewable energy resources. This method can target two goals. The first option increases the use of local decentralized energy resources (DER) such as solar panels or other local generators (microgrid). The second option reduces CO2 emissions (greenhouse gas emissions) by analyzing the energy mix (fuel, coal, solar, wind, and others) of the grid.

Cost Optimizer: Reduces the energy supply costs for EV charging. This method uses price information such as wholesale prices, energy rates, or generation costs to reduce the costs for EV charging. It can target either the charging costs of an individual user or of an entire charging network.

United Chargers also offers various options to integrate stationary batteries as well as V2G commands.

## Get started
This documentation explains the functionality of the optimization, the API, and its integration in central charging systems such as OCPP-servers.

The United Chargers API is the simplest and recommended way to generate and receive optimized load profiles or single limits.

To get started, send us a message to contact@unitedchargers.com or through the website.

Below you find a description of possible data types that may be required — contact United Chargers for more information or help.

The following sections describe the functionality of the API.

### Base Url
https://engine.unitedchargers/CustomerApi/1.0/
Copy to clipboardErrorCopiedCopy to clipboardErrorCopiedCopy to clipboardErrorCopiedCopy to clipboardErrorCopied
Authentication
The United Chargers API uses API keys to authenticate requests.

The authentication is performed through the Bearer Authentication scheme. Any API call needs to provide an access token that matches the network or EVSE. You must supply the access token as in the authorization request header field. You can request a new access token by messaging to contact@unitedchargers.com.

Content-type: application/json

### API Tutorial
#### 1: Verify the access to your API Key
Before going any further on your code implementation, go on http://acharger.ca.

Use your credentials and password to connect to it. If you connect for the first time, make sure that you can access your API_KEY in the profile page.

To do so:

1° Click on your profile picture
2° Click Profile
Copy to clipboardErrorCopiedCopy to clipboardErrorCopiedCopy to clipboardErrorCopiedCopy to clipboardErrorCopied
You should see your API key just right on this page.

#### 2: Add a new network
Once you have access to your API key, you add a new network to be able to send a request. (if United Chargers already set up everything for you, you can skip this step)

To do so:

1° Go on the network page
2° On the network page click on "Add network" or on the "+" icon
3° Fill the fields as requested in the form
4° In "Optimization method" select "unpredictive peak shaving" if you didn't import baseload or PV date
5° Click "Create" or the ok-button
Copy to clipboardErrorCopiedCopy to clipboardErrorCopiedCopy to clipboardErrorCopiedCopy to clipboardErrorCopied
#### 3: Add a new charger
Cool, now that you have your first network on United Chargers, let's add a charging station (= EVSE). (if United Chargers already set up everything for you, you can skip this step)

To do so:

1° Stay on the same page
2° Select the new network in the table
3° On the box just next to the network box click "Add EVSE" or on the "+" icon
4° Fill the fields as requested in the form
5° Click "Create" or the ok-button
Copy to clipboardErrorCopiedCopy to clipboardErrorCopiedCopy to clipboardErrorCopiedCopy to clipboardErrorCopied
#### 4: Add a new connector
So far, so good. You now have an API key, a network, and a charger. Let's finish all this by adding a connector for the charging station. (if United Chargers already set up everything for you, you can skip this step)

To do so:

1° Click on the new charging station that appeared in the list just after you created it
2° You're now in the charger page, go into the "Connectors" Card
3° Repeat the same procedure that you did previously
Copy to clipboardErrorCopiedCopy to clipboardErrorCopiedCopy to clipboardErrorCopiedCopy to clipboardErrorCopied
#### 5: Sending your first request
Brilliant, now we have everything set up in our system to allow you to trigger some optimizations.

So let's send a request to our API and visualize the data on the dashboard.

To do so:

use your favorite rest client to send the request with the following header and body

Endpoint
```html
https://engine.unitedchargers/CustomerApi/1.0/run_optimization
```
Header
```html
Accept: */*
Authorization: Bearer YOUR_API_KEY
Cache-Control: no-cache
Connection: keep-alive
accept-encoding: gzip, deflate
Content-Type: application/json
Copy to clipboardErrorCopiedCopy to clipboardErrorCopiedCopy to clipboardErrorCopiedCopy to clipboardErrorCopied
Body

{
  "chargePointId": "YOUR_CHARGE_POINT_ID",
  "connectorId": 1
}
```
#### 6: Verify the result
Your API response should look similar to the one displayed in the section "run an optimization". The API response contains a field "csChargingProfiles", which is OCPP-compliant and can be forwarded to the charging station.

You can verify that you got a 200 as a response. Verify also the visualized result in the dashboard.

#### 7: Integrate the request in your code
You can now implement the different endpoints that we provide and automatically trigger optimizations once a user connects a vehicle to a charging station.

To automatically receive new limits (when United Chargers re-optimizes) or to receive limit values instead of profiles, please implement the endpoints that we described below.

In case you have any trouble don't hesitate to contact our support with our mail address: support@unitedchargers.com

## Endpoints


## Dashboard
<div align=center>
<img width="1000" src="assets/img/cover.jpg"/>
</div>

## Demo

A basic tab set using the default [options](#options).

<!-- tabs:start -->

#### **Tab A**

This is some text.

* List item A-1
* List item A-2

```js
// JavaScript
function add(a, b) {
  return a + b;
}
```

#### **Tab B**

### This is a heading {docsify-ignore}

This is some text.

* List item B-1
* List item B-2

```css
/* CSS */
body {
  background: white;
  color: #222;
}
```

#### **Tab C**

### This is a heading {docsify-ignore}

This is some text.

* List item C-1
* List item C-2

```html
<!-- HTML -->
<h1>Heading</h1>
<p>This is a paragraph.</p>
```

<!-- tabs:end -->

?> Like docsify-tabs? Be sure to check out [docsify-themeable](https://jhildenbiddle.github.io/docsify-themeable) for your site theme!

## Features

* Generate tabbed content using unobtrusive markup
* Persist tab selections on refresh/revisit
* Sync tab selection for tabs with matching labels
* Style tabs using "classic" or "material" tab theme
* Customize styles using CSS custom properties
* Compatible with [docsify-themeable](https://jhildenbiddle.github.io/docsify-themeable/) themes

**Limitations**

* Nested tabsets (i.e. tabs within tabs) are not supported.
* Tabs wraps when their combined width exceeds the content area width.

## Installation

1. Add the docsify-tabs plugin to your `index.html` after docsify.

   ```html
   <!-- docsify (latest v4.x.x)-->
   <script src="https://cdn.jsdelivr.net/npm/docsify@4"></script>

   <!-- docsify-tabs (latest v1.x.x) -->
   <script src="https://cdn.jsdelivr.net/npm/docsify-tabs@1"></script>
   ```

1. Review the [Options](#options) section and configure as needed.

   ```javascript
   window.$docsify = {
     // ...
     tabs: {
       persist    : true,      // default
       sync       : true,      // default
       theme      : 'classic', // default
       tabComments: true,      // default
       tabHeadings: true       // default
     }
   };
   ```

1. Review the [Customization](#customization) section and set theme properties as needed.

   ```html
   <style>
     :root {
       --docsifytabs-border-color: #ededed;
       --docsifytabs-tab-highlight-color: purple;
     }
   </style>
   ```

## Usage

1. Define a tab set using `tabs:start` and `tabs:end` HTML comments.

   HTML comments are used to mark the start and end of a tab set. The use of HTML comments prevents tab-related markup from being displayed when markdown is rendered as HTML outside of your docsify site (e.g. GitHub, GitLab, etc).

   ```markdown
   <!-- tabs:start -->

   ...

   <!-- tabs:end -->
   ```

1. Define tabs within a tab set using heading + bold markdown.

   Heading text will be used as the tab label, and all proceeding content will be associated with that tab up to start of the next tab or a `tab:end` comment. The use of heading + bold markdown allows tabs to be defined using standard markdown and ensures that tab content is displayed with a heading when rendered outside of your docsify site (e.g. GitHub, GitLab, etc).

   ```markdown
   <!-- tabs:start -->

   #### ** English **

   Hello!

   #### ** French **

   Bonjour!

   #### ** Italian **

   Ciao!

   <!-- tabs:end -->
   ```

   See [`options.tabHeadings`](#tabheadings) for details or [`options.tabComments`](#tabcomments) for an alternate method of defining tabs within tab sets.

1. Voilà! A tab set is formed.

   <!-- tabs:start -->

   #### **English**

   Hello!

   #### **French**

   Bonjour!

   #### **Italian**

   Ciao!

   <!-- tabs:end -->

## Options

Options are set within the [`window.$docsify`](https://docsify.js.org/#/configuration) configuration under the `tabs` key:

```html
<script>
  window.$docsify = {
    // ...
    tabs: {
      persist    : true,      // default
      sync       : true,      // default
      theme      : 'classic', // default
      tabComments: true,      // default
      tabHeadings: true       // default
    }
  };
</script>
```

### persist

* Type: `boolean`
* Default: `true`

Determines if tab selections will be restored after a page refresh/revisit.

**Configuration**

```javascript
window.$docsify = {
  // ...
  tabs: {
    persist: true // default
  }
};
```

### sync

* Type: `boolean`
* Default: `true`

Determines if tab selections will be synced across tabs with matching labels.

**Configuration**

```javascript
window.$docsify = {
  // ...
  tabs: {
    sync: true // default
  }
};
```

**Demo**

<!-- tabs:start -->

#### **macOS**

Instructions for macOS...

#### **Windows**

Instructions for Windows...

#### **Linux**

Instructions for Linux...

<!-- tabs:end -->

<!-- tabs:start -->

#### **macOS**

More instructions for macOS...

#### **Windows**

More instructions for Windows...

#### **Linux**

More instructions for Linux...

<!-- tabs:end -->

### theme

* Type: `string|boolean`
* Accepts: `'classic'`, `'material'`, `false`
* Default: `'classic'`

Sets the tab theme. A value of `false` will indicate that no theme should be applied, which should be used when creating custom tab themes.

**Configuration**

```javascript
window.$docsify = {
  // ...
  tabs: {
    theme: 'classic' // default
  }
};
```

**Demo**

<label data-class-target="label + .docsify-tabs" data-class-remove="docsify-tabs--material" data-class-add="docsify-tabs--classic">
  <input name="theme" type="radio" value="classic" checked="checked"> Classic
</label>
<label data-class-target="label + .docsify-tabs" data-class-remove="docsify-tabs--classic" data-class-add="docsify-tabs--material">
  <input name="theme" type="radio" value="material"> Material
</label>
<label data-class-target="label + .docsify-tabs" data-class-remove="docsify-tabs--classic docsify-tabs--material">
  <input name="theme" type="radio" value="none"> No Theme
</label>

<!-- tabs:start -->

#### **Tab A**

This is some text.

#### **Tab B**

This is some more text.

#### **Tab C**

Yes, this is even more text.

<!-- tabs:end -->

### tabComments

* Type: `boolean`
* Default: `true`

Determines if tabs within a tab set can be defined using tab comments.

Note that defining tabs using HTML comments means tab content will not be labeled when rendered outside of your docsify site (e.g. GitHub, GitLab, etc). For this reason, defining tabs using [`options.tabHeadings`](#tabheadings) is recommended.

**Configuration**

```javascript
window.$docsify = {
  // ...
  tabs: {
    tabComments: true // default
  }
};
```

**Example**

```markdown
<!-- tabs:start -->

<!-- tab:English -->

Hello!

<!-- tab:French -->

Bonjour!

<!-- tab:Italian -->

Ciao!

<!-- tabs:end -->
```

### tabHeadings

* Type: `boolean`
* Default: `true`

Determines if tabs within a tab set can be defined using heading + bold markdown.

The use of heading + bold markdown allows tabs to be defined using standard markdown and ensures that tab content is displayed with a heading when rendered outside of your docsify site (e.g. GitHub, GitLab, etc). Heading levels 1-6 are supported (e.g. `#` - `######`) as are both asterisks (`**`) and underscores (`__`) for bold text via markdown.

**Configuration**

```javascript
window.$docsify = {
  // ...
  tabs: {
    tabHeadings: true // default
  }
};
```

**Example**

```markdown
<!-- tabs:start -->

#### ** English **

Hello!

#### ** French **

Bonjour!

#### ** Italian **

Ciao!

<!-- tabs:end -->
```

## Customization

### Themes

See [`options.theme`](#theme) for details on available themes.

### Theme Properties

Theme properties allow you to customize tab styles without writing complex CSS. The following list contains the default theme values.

[vars.css](https://raw.githubusercontent.com/jhildenbiddle/docsify-tabs/master/src/css/vars.css ':include :type:code')

To set theme properties, add a `<style>` element to your `index.html` file after all other stylesheets and set properties within a `:root` selector.

```html
<style>
  :root {
    --docsifytabs-border-color: #ededed;
    --docsifytabs-tab-highlight-color: purple;
  }
</style>
```

### Custom Styles

Custom tab styles can be created using docsify-tabs CSS classes and `data-tab` attributes.

Use the `docsify-tabs` CSS classes to apply common tab styles to tab blocks, toggles, and content containers. Use `data-tab` attributes to apply styles to specific tabs and/or content based on the tab label.

```html
<div class="docsify-tabs docsify-tabs--classic">
  /* Tab A */
  <button class="docsify-tabs__tab docsify-tabs__tab--active" data-tab="tab a">Tab A</button>
  <div class="docsify-tabs__content" data-tab-content="tab a">
    ...
  </div>

  /* Tab B */
  <button class="docsify-tabs__tab" data-tab="tab b">Tab B</button>
  <div class="docsify-tabs__content" data-tab-content="tab b">
    ...
  </div>
</div>
```

**Examples**

<!-- tabs:start -->

#### **Icon Only**

**Tab Markdown**

```markdown
#### **Icon Only**
```

**Custom CSS**

```css
.docsify-tabs__tab[data-tab="icon only"] {
  letter-spacing: -100vw;
  color: transparent;
}
.docsify-tabs__tab[data-tab="icon only"]:before {
  content: url(path/to/image.png);
  vertical-align: middle;
}
```

#### **Icon + Label**

**Tab Markdown**

```markdown
#### **Icon + Label**
```

**Custom CSS**

```css
[data-tab="icon + label"]:before {
  content: url(path/to/image.png);
  height: 16px;
  margin-right: 0.5em;
  vertical-align: middle;
}
```

#### **Active State**

**Tab Markdown**

```markdown
#### **Active State**
```

**Custom CSS**

```css
.docsify-tabs__tab--active[data-tab="active state"] {
  box-shadow: none;
  background: #13547a;
  color: white;
}
.docsify-tabs__content[data-tab-content="active state"] {
  background-image: linear-gradient(0deg, #80d0c7 0%, #13547a 100%);
}
.docsify-tabs__content[data-tab-content="active state"] p strong {
  color: white;
}
```

#### **CodePen**

<div class="codepen" data-height="400" data-theme-id="light" data-default-tab="html,result" data-user="jhildenbiddle" data-slug-hash="wGjZLj" style="height: 400px; height: 70vh; box-sizing: border-box; display: flex; align-items: center; justify-content: center; border: 0; " data-pen-title="Demo: Vertical Rhythm Reset (CSS/SCSS)"></div>
<script async src="https://static.codepen.io/assets/embed/ei.js"></script>

**Tab Markdown**

```markdown
#### **CodePen**
```

**Custom CSS**

```css
[data-tab-content="codepen"] .cp_embed_wrapper {
  position: relative;
  top: calc(0px - var(--docsifytabs-content-padding));
  left: calc(0px - var(--docsifytabs-content-padding));
  width: calc(100% + calc(var(--docsifytabs-content-padding) * 2));
  margin-bottom: calc(0px - var(--docsifytabs-content-padding));
}
```

#### **Badge**

**Tab Markdown**

```markdown
#### **Badge**
```

**Custom CSS**

```css
[data-tab="badge"]:after {
  content: 'New!';
  position: absolute;
  top: 0;
  right: 0;
  min-height: 1em;
  min-width: 1em;
  padding: 0.325em 0.5em 0.3em 0.5em;
  border-radius: 3px;
  background: red;
  color: #fff;
  font-family: sans-serif;
  font-weight: bold;
  font-size: 11px;
  line-height: 1;
  transform: translate(35%, -45%);
}
```

<!-- tabs:end -->

## Contact & Support

* Create a [GitHub issue](https://github.com/jhildenbiddle/docsify-tabs/issues) for bug reports, feature requests, or questions
* Follow [@jhildenbiddle](https://twitter.com/jhildenbiddle) for announcements
* Add a ⭐️ [star on GitHub](https://github.com/jhildenbiddle/docsify-tabs) or ❤️ [tweet](https://twitter.com/intent/tweet?url=https%3A%2F%2Fgithub.com%2Fjhildenbiddle%2Fdocsify-tabs&hashtags=css,developers,frontend,javascript) to support the project!

## License

This project is licensed under the [MIT license](https://github.com/jhildenbiddle/docsify-tabs/blob/master/LICENSE).

Copyright (c) John Hildenbiddle ([@jhildenbiddle](https://twitter.com/jhildenbiddle))

<!-- GitHub Buttons -->
<script async defer src="https://buttons.github.io/buttons.js"></script>
