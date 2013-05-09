<?php

/**
 * @file
 * Yottaa Cache include file, to be added to settings.php file.
 */

/**
 * Yottaa cache implementation.
 */
class YottaaCache implements DrupalCacheInterface {
  protected $bin;

  function __construct($bin) {
    $this->bin = $bin;
  }

  function get($cid) {
    return FALSE;
  }

  function getMultiple(&$cids) {
    return array();
  }

  function set($cid, $data, $expire = CACHE_PERMANENT) {
  }

  function clear($cid = NULL, $wildcard = FALSE) {
    global $user;
    watchdog('yottaa', 'Clear method is invoked with @cid.', array('@cid' => $cid), WATCHDOG_DEBUG);
    // Check if Yottaa module installed.
    if (!module_exists('yottaa')) {
      return;
    }

    if (class_exists('YottaaDrupalAPI')) {
      $yottaa_api = yottaa_api_drupal();

      if (empty($cid)) {
        watchdog('yottaa', 'Clear request with empty cid.', array(), WATCHDOG_DEBUG);
        /*
        if (variable_get('cache_lifetime', 0)) {
          $user->cache = REQUEST_TIME;
          $cache_flush = variable_get('cache_flush_' . $this->bin, 0);
          if ($cache_flush == 0) {
            variable_set('cache_flush_' . $this->bin, REQUEST_TIME);
          }
          elseif (REQUEST_TIME > ($cache_flush + variable_get('cache_lifetime', 0))) {
            $yottaa_api->flush();
            variable_set('cache_flush_' . $this->bin, 0);
          }
        }
        else {
          $yottaa_api->flush();
        }
        */
      }
      else {
        if ($wildcard) {
          if ($cid == '*') {
            $yottaa_api->flush();
          }
          else {
            $path = $cid . '(.*)';
            $yottaa_api->flushPaths(array(array("condition" => $path, "name" => "URI", "operator" => "REGEX")));
          }
        }
        elseif (is_array($cid)) {
          yottaa_expire_cache($cid);
        }
        else {
          yottaa_expire_cache(array($cid));
        }
      }
    }
  }

  function isEmpty() {
    return FALSE;
  }
}
