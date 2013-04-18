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

    You can then install the module with the generated zip file.

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

## Links ##

* [Yottaa](http://www.yottaa.com)
* [Drupal](http://www.drupal.org/)