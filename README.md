# Yottaa eComet Plugin for Drupal (Beta)

Yottaa Site Optimizer will speed up your Drupal website automatically.  Faster sites have lower bounce rate, higher conversion rate, and more sales.

Whether you're already a Yottaa Site Optimizer user or want to try it for the first time, you'll enjoy the ease of having a Yottaa control panel right on your Drupal admin panel. Plugin users also have access to special caching features only available through the Drupal eComet plugin, which can improve page speed even beyond Yottaa Site Optimizer's standard techniques.

## Plugin Setup ##

[Setup Guide](http://www.yottaa.com/reference-materials/yottaa-ecomet-plugin-for-drupal-beta/)

## Build Plugin ##

1. Install Ant

    Install and add [required jars](http://ant.apache.org/manual/Tasks/scp.html) for scp task.

2. Build Yottaa Module

    Once you clone the repository, run following ant command to build the Yottaa module

    ```
    ant package
    ```

    You can then install the module with the generated zip files under the build directory.

3. Setup Dev Environment for Yottaa Module

    If you have local or remote installation of Drupal and you want update the module constantly, you can add a custom-build.properties file right under the root directory of your copy of github project.

    Put following configurations in the properties file and replace the values with your own settings

    ```
    drupal.location=[Root directory of your local Drupal installation]
    scp.drupal.host=[Server IP for your remote Drupal installation]
    scp.drupal.username=[Username for accessing your server]
    scp.drupal.password=[Password for accessing your server]
    scp.drupal.basepath=[Root directory of your remote Drupal installation]
    ```

    You can then run

    ```
    ant dev
    ```
    to update your local Drupal installation.

    or

    ```
    ant publish
    ```
    to update your remote Drupal installation.

## Install Plugin ##

1. Depends on the version of your Drupal installation, download either [yottaa-7.x-1.0.tar.gz](https://github.com/Yottaa/e-comet-drupal/blob/master/dist/yottaa-7.x-1.0.tar.gz?raw=true) for Drupal 7.x or [yottaa-6.x-1.0.tar.gz](https://github.com/Yottaa/e-comet-drupal/blob/master/dist/yottaa-6.x-1.0.tar.gz?raw=true) for Drupal 6.x

2. Unzip it onto the [Directory of your Drupal installation]/sites/all/modules directory.

3. Find settings.php, which is under the [Directory of your Drupal installation]/sites/default directory, and add the following lines:

   ```
   $conf['reverse_proxy'] = TRUE;
   $conf['reverse_proxy_addresses'] = isset($_SERVER['REMOTE_ADDR']) ? array($_SERVER['REMOTE_ADDR']) : array();
   ```

4. If you want to make Yottaa as your default caching backend, add the following lines to the settings.php as well

   ```
   $conf['cache_backends'] = array('sites/all/modules/yottaa/yottaa.cache.inc');
   $conf['cache_class_cache_page'] = 'YottaaCache';
   $conf['page_cache_invoke_hooks'] = FALSE;
   ```

5. Log into your Drupal admin panel. Go to the "Modules" page and you should see Yottaa has appeared under a "Web Peformance" category. Hit enable and save the configuration.

6. Download, install and enable [Drupal Cache Expiration Module](http://drupal.org/project/expire).

7. Now go to your configuration panel and click to manage Yottaa settings.

   1. If you are creating a new Yottaa account, fill out the the form shown in the image below. (Some of the fields should be pre-populated, but fill out any blank ones). Once that information is saved your Yottaa account will be created, but will be in "Preview" mode. You can then either click the "Yottaa Dashboard" link or go to Yottaa and log in.  There, follow the steps to activate Yottaa. Once you activate, you'll have a free 7 day trial, but this will become your two month free service once you email sales@yottaa.com

   2. If you are adding an existing Yottaa account, click the tab for "Exisiting Yottaa Account" and you'll see a form to fill your Site ID, API Key, and User ID.

   3. To find these, open a new tab and log into [Yottaa](http://apps.yottaa.com). Once logged in you will see a string of letters and numbers in the URL.  This is your site ID. Next, in the left navigation click "Settings" and "API Key". This page will display both your API Key and User ID. Paste these three numbers into the fields on the Drupal configuration panel.

   4. Scroll down the Yottaa module page . At the bottom there is a checklist of four items. To complete these, return to [Yottaa](http://apps.yottaa.com).

   5. Click the Site Optimizer tab, and click CDN and Cache Control in the left navigation. Set the recommended setting listed in the Yottaa Optimization Settings section of this page.

   6. Return to the Drupal eComet plugin and confirm that the site is listed as "Live" and checklist items have changed to "Passed".  Here you can also make sure the box is checked for "Automatically clear Yottaa's Site Optimizer cache on node changes. (This will automatically send the most updated version of your site to Yottaa's CDN nodes, ensuring that your visitors have the fastest load times possible).

## Yottaa Optimization Settings ##

1. Enable Asset Caching with all default settings.

2. Enable HTML Caching with following settings

   1. HTML Caching settings:

      Default caching behavior - follow HTTP cache control header when possible

   2. Additional Settings:

      Enable home page caching rule.

      If request URL contains query string: Unique cache

   3. Exceptions to HTML Caching:

      If URI contains "/edit"
      Then exclude this resource from optimization.

      If URI contains "admin/"
      Then exclude this resource from optimization.

      If URI contains "admin%252F"
      Then exclude this resource from optimization.

      If URI matches RegExp "(.*)"
      and Request-Header whose name equals to "Cookie" and whose value contains "SESS"
      Then exclude this resource from optimization.


## Links ##

* [Yottaa](http://www.yottaa.com)
* [Drupal](http://www.drupal.org/)