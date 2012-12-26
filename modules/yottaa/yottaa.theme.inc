<?php

function theme_yottaa_info($variables)
{
    // Render output
    drupal_add_css(drupal_get_path('module', 'yottaa') . '/css/yottaa.css');
    $contents = '<h1>' . theme('image', array('path' => drupal_get_path('module', 'yottaa') . '/images/yottaa.png')) . '<strong>My Yottaa Page</strong></h1>';

    $yottaa_site_id = variable_get('yottaa_site_id', '');
    $yottaa_user_id = variable_get('yottaa_user_id', '');
    $yottaa_api_key = variable_get('yottaa_api_key', '');

    $json_output = check_yottaa_status();
    $yottaa_status = $json_output["optimizer"];

    if (!isset($json_output["error"])) {
        $yottaa_preview_url = $json_output["preview_url"];
        if ($yottaa_status == 'preview') {
            $contents .= '<div>';
            $contents .= 'Your site is currently in <span class="status-preview">' . $yottaa_status . '</span>. This allows you to access an optimized';
            $contents .= ' version of your website using the preview URL (<a href="' . $yottaa_preview_url . '" target="_blank">' . $yottaa_preview_url . '</a>).';
            $contents .= ' Before making your site live look over the links and configuration below.';
            $contents .= '</div>';
        } else if ($yottaa_status == 'live') {
            $contents .= '<div>';
            $contents .= 'Your site is currently in <span class="status-live">Live</span>.';
            $contents .= '</div>';
        } else if ($yottaa_status == 'paused') {
            $contents .= '<div>';
            $contents .= 'Your site is currently in <span class="status-paused">Paused</span>.';
            $contents .= '</div>';
        }
    } else {
        $error = $json_output["error"];
        $contents .= '<div class="status-error">Error: ' . json_encode($error) . '</div>';
    }

    $contents .= '<h3>Links</h3>';
    $contents .= '<div><a href="https://apps.yottaa.com/" target="_blank">Yottaa Dashboard</a><div>';
    $contents .= '<div><a href="https://apps.yottaa.com/framework/web/sites/' . $yottaa_site_id . '/optimizer" target="_blank">Yottaa Site Overview</a><div>';
    $contents .= '<div><a href="https://apps.yottaa.com/framework/web/sites/' . $yottaa_site_id . '/settings" target="_blank">Yottaa Optimization Configuration</a><div>';

    $contents .= '<h3>Actions</h3>';
    if (!isset($json_output["error"])) {
        if ($yottaa_status == 'preview') {
            $contents .= drupal_render((drupal_get_form('yottaa_optimizer_activate_form')));
            $contents .= drupal_render((drupal_get_form('yottaa_clear_cache_form')));
        } else if ($yottaa_status == 'live') {
            $contents .= drupal_render((drupal_get_form('yottaa_optimizer_pause_form')));
            $contents .= drupal_render((drupal_get_form('yottaa_clear_cache_form')));
        } else if ($yottaa_status == 'paused') {
            $contents .= drupal_render((drupal_get_form('yottaa_optimizer_resume_form')));
            $contents .= drupal_render((drupal_get_form('yottaa_clear_cache_form')));
        }
    }

    $contents .= '<h3>Settings</h3>';
    $contents .= '<div class="setting"><span>User Id</span>' . $yottaa_user_id .'<div>';
    $contents .= '<div class="setting"><span>API Key</span>' . $yottaa_api_key .'<div>';
    $contents .= '<div class="setting"><span>Site Id</span>' . $yottaa_site_id .'<div>';
    $contents .= '<div class="setting">' . l(t('Advanced Configuration'), 'admin/settings/yottaa/existing_user') . '</div>';

    $contents .= '<h3>Checklist</h3>';

    return $contents;
}

/**
 * Custom theme for yottaa optimizer activate form.
 *
 * @param $form
 */
function theme_yottaa_optimizer_activate_form($variables) {
  $form = $variables['form'];

  $rows = array(array(drupal_render($form['submit']),t('Activating your site allows all e-commerce visitors to receive the benefits out Yottaa\'s site speed optimizer.')));

  return theme('table', array('rows' => $rows)) . drupal_render_children($form);
}

/**
 * Custom theme for yottaa optimizer pause form.
 *
 * @param $form
 */
function theme_yottaa_optimizer_pause_form($variables) {
  $form = $variables['form'];

  $rows = array(array(drupal_render($form['submit']),t('Activating your site allows all e-commerce visitors to receive the benefits out Yottaa\'s site speed optimizer.')));

  return theme('table', array('rows' => $rows)) . drupal_render_children($form);
}

/**
 * Custom theme for yottaa optimizer resume form.
 *
 * @param $form
 */
function theme_yottaa_optimizer_resume_form($variables) {
  $form = $variables['form'];

  $rows = array(array(drupal_render($form['submit']),t('Starting optimization will apply optimizations on your website within 5minutes.')));

  return theme('table', array('rows' => $rows)) . drupal_render_children($form);
}

/**
 * Custom theme for yottaa clear cache form.
 *
 * @param $form
 */
function theme_yottaa_clear_cache_form($variables) {
  $form = $variables['form'];

  $rows = array(array(drupal_render($form['submit']),t('Clearing the cache will remove all of your sites resources from our CDN. Use this option if you have updated a resource (gif, css, JavaScript).')));

  return theme('table', array('rows' => $rows)) . drupal_render_children($form);
}