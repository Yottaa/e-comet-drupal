<?php
/**
 * @file
 *
 */
/**
 * @param $variables
 * @return string
 */
function theme_yottaa_info($variables) {
  // Render output
  $yottaa_api = yottaa_api_drupal();

  drupal_add_css(drupal_get_path('module', 'yottaa') . '/css/yottaa.css');
  $contents = '<h1>' . theme_image(drupal_get_path('module', 'yottaa') . '/images/yottaa.png') . '<strong>My Yottaa Page</strong></h1>';

  $parameters = $yottaa_api->getParameters();
  $yottaa_site_id = $parameters['site_id'];
  $yottaa_user_id = $parameters['user_id'];
  $yottaa_api_key = $parameters['api_key'];

  $json_output = $yottaa_api->getStatus();
  $yottaa_status = $json_output["optimizer"];

  if (!isset($json_output["error"])) {
    $yottaa_preview_url = $json_output["preview_url"];
    if ($yottaa_status == 'preview') {
      $contents .= '<div>';
      $contents .= 'Your site is currently in <span class="status-preview">' . $yottaa_status . '</span>. This allows you to access an optimized';
      $contents .= ' version of your website using the preview URL (<a href="' . $yottaa_preview_url . '" target="_blank">' . $yottaa_preview_url . '</a>).';
      $contents .= ' Before making your site live look over the links and configuration below.';
      $contents .= '</div>';
    }
    elseif ($yottaa_status == 'live') {
      $contents .= '<div>';
      $contents .= 'Your site is currently in <span class="status-live">Live</span>.';
      $contents .= '</div>';
    }
    elseif ($yottaa_status == 'paused') {
      $contents .= '<div>';
      $contents .= 'Your site is currently in <span class="status-paused">Paused</span>.';
      $contents .= '</div>';
    }
  }
  else {
    $error = $json_output["error"];
    $contents .= '<div class="status-error">Error: ' . json_encode($error) . '</div>';
  }

  $contents .= '<h3>Links</h3>';
  $contents .= '<div><a href="https://apps.yottaa.com/" target="_blank">Yottaa Dashboard</a></div>';
  $contents .= '<div><a href="https://apps.yottaa.com/framework/web/sites/' . $yottaa_site_id . '/optimizer" target="_blank">Yottaa Site Overview</a></div>';
  $contents .= '<div><a href="https://apps.yottaa.com/framework/web/sites/' . $yottaa_site_id . '/settings" target="_blank">Yottaa Optimization Configuration</a></div>';

  $contents .= '<h3>Actions</h3>';
  if (!isset($json_output["error"])) {
    if ($yottaa_status == 'preview') {
      $contents .= drupal_get_form('yottaa_optimizer_activate_form');
      $contents .= '<div>Activating your site allows all e-commerce visitors to receive the benefits out Yottaa\'s site speed optimizer.</div>';
    }
    elseif ($yottaa_status == 'live') {
      $contents .= drupal_get_form('yottaa_optimizer_pause_form');
      $contents .= '<div>Activating your site allows all e-commerce visitors to receive the benefits out Yottaa\'s site speed optimizer.</div>';
    }
    elseif ($yottaa_status == 'paused') {
      $contents .= drupal_get_form('yottaa_optimizer_resume_form');
      $contents .= '<div>Starting optimization will apply optimizations on your website within 5minutes.</div>';
    }
    $contents .= drupal_get_form('yottaa_clear_cache_form');
    $contents .= '<div>Clearing the cache will remove all of your sites resources from our CDN. Use this option if you have updated a resource (gif, css, JavaScript).</div>';
  }

  $contents .= '<h3>Settings</h3>';
  $contents .= drupal_get_form('yottaa_auto_clear_cache_form');
  $contents .= '<div class="setting"><span class="setting-item-title">User Id</span>' . $yottaa_user_id . '</div>';
  $contents .= '<div class="setting"><span class="setting-item-title">API Key</span>' . $yottaa_api_key . '</div>';
  $contents .= '<div class="setting"><span class="setting-item-title">Site Id</span>' . $yottaa_site_id . '</div>';
  $contents .= '<div class="setting">' . l(t('Advanced Configuration'), 'admin/config/system/yottaa/existing_user') . '</div>';

  $contents .= '<h3>Checklist</h3>';

  $settings = $yottaa_api->getSettings();

  if (!isset($settings["error"])) {
    $contents .= '<div class="checklist-items">';
    // Site pages
    $contents .= '<div class="checklist-item"><span class="checklist-item-title">Enable caching of site pages</span>';
    if ($settings["site_pages_caching"] == 'included') {
      $contents .= '<span class="checklist-item-status checklist-item-status-passed">Passed</span></div>';
    }
    else {
      $contents .= '<span class="checklist-item-status checklist-item-status-failed">Failed</span></div>';
      $contents .= '<div class="checklist-item"><span class="checklist-item-helper">Turn on <a href="https://apps.yottaa.com/framework/web/sites/' . $yottaa_site_id . '/settings?tab=1" target="_blank">HTML Caching</a>.</span></div>';
    }

    // Only cache pages for anonymous users
    $contents .= '<div class="checklist-item"><span class="checklist-item-title">Only cache pages for anonymous users</span>';
    if ($settings["only_cache_anonymous_users"] == 'excluded') {
      $contents .= '<span class="checklist-item-status checklist-item-status-passed">Passed</span></div>';
    }
    else {
      $contents .= '<span class="checklist-item-status checklist-item-status-failed">Failed</span></div>';
      $contents .= '<div class="checklist-item"><span class="checklist-item-helper">Add an exception into the <a href="https://apps.yottaa.com/framework/web/sites/' . $yottaa_site_id . '/settings?tab=1" target="_blank">HTML Caching</a> which excludes resource from optimization if Request-Header whose name equals to "Cookie" and whose value contain "SESS".</span></div>';
    }

    // Edit pages
    $contents .= '<div class="checklist-item"><span class="checklist-item-title">Disable caching of node editing pages</span>';
    if ($settings["edit_pages_caching"] == 'excluded') {
      $contents .= '<span class="checklist-item-status checklist-item-status-passed">Passed</span></div>';
    }
    else {
      $contents .= '<span class="checklist-item-status checklist-item-status-failed">Failed</span></div>';
      $contents .= '<div class="checklist-item"><span class="checklist-item-helper">Add an exclude exception into the <a href="https://apps.yottaa.com/framework/web/sites/' . $yottaa_site_id . '/settings?tab=1" target="_blank">HTML Caching</a> with a "contains" expression of <strong>/edit</strong>.</span></div>';
    }

    // Configure pages
    $contents .= '<div class="checklist-item"><span class="checklist-item-title">Disable caching of configuration pages</span>';
    if ($settings["configure_pages_caching"] == 'excluded') {
      $contents .= '<span class="checklist-item-status checklist-item-status-passed">Passed</span></div>';
    }
    else {
      $contents .= '<span class="checklist-item-status checklist-item-status-failed">Failed</span></div>';
      $contents .= '<div class="checklist-item"><span class="checklist-item-helper">Add an exclude exception into the <a href="https://apps.yottaa.com/framework/web/sites/' . $yottaa_site_id . '/settings?tab=1" target="_blank">HTML Caching</a> with a "contains" expression of <strong>admin/</strong>.</span></div>';
      $contents .= '<div class="checklist-item"><span class="checklist-item-helper">Add an exclude exception into the <a href="https://apps.yottaa.com/framework/web/sites/' . $yottaa_site_id . '/settings?tab=1" target="_blank">HTML Caching</a> with a "contains" expression of <strong>admin%252F</strong>.</span></div>';
    }

    // Proxy mode
    $contents .= '<div class="checklist-item"><span class="checklist-item-title">Enable drupal proxy mode</span>';
    if (variable_get('reverse_proxy', 0) == 0 && $yottaa_status == 'live') {
      $contents .= '<span class="checklist-item-status checklist-item-status-failed">Failed</span></div>';
      $contents .= '<div class="checklist-item"><span class="checklist-item-helper">Need to enable the proxy mode for drupal using the form in Settings section.</span></div>';
    }
    else {
      $contents .= '<span class="checklist-item-status checklist-item-status-passed">Passed</span></div>';
    }
    $contents .= '</div>';

  }
  else {
    $contents .= '<div class="checklist-error">' . json_encode($error) . '</div>';
  }
  return $contents;
}

/**
 * Custom theme for yottaa optimizer activate form.
 *
 * @param $form
 */
function theme_yottaa_optimizer_activate_form($variables) {
  return $variables['form'];
}

/**
 * Custom theme for yottaa optimizer pause form.
 *
 * @param $form
 */
function theme_yottaa_optimizer_pause_form($variables) {
  return $variables['form'];
}

/**
 * Custom theme for yottaa optimizer resume form.
 *
 * @param $form
 */
function theme_yottaa_optimizer_resume_form($variables) {
  return $variables['form'];
}

/**
 * Custom theme for yottaa clear cache form.
 *
 * @param $form
 */
function theme_yottaa_clear_cache_form($variables) {
  return $variables['form'];
}

/**
 * Custom theme for yottaa auto clear cache setting form.
 *
 * @param $form
 */
function theme_yottaa_auto_clear_cache_form($variables) {
  return $variables['form'];
}