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
    if (empty($cid) && !module_exists('expire')) {
      watchdog('yottaa', 'Clear request without cid.', array(), WATCHDOG_DEBUG);
      if (variable_get('cache_lifetime', 0)) {
        $user->cache = REQUEST_TIME;
        $cache_flush = variable_get('cache_flush_' . $this->bin, 0);
        if ($cache_flush == 0) {
          variable_set('cache_flush_' . $this->bin, REQUEST_TIME);
        }
        elseif (REQUEST_TIME > ($cache_flush + variable_get('cache_lifetime', 0))) {
          yottaa_flush_yottaa_cache_all();
          variable_set('cache_flush_' . $this->bin, 0);
        }
      }
      else {
        yottaa_flush_yottaa_cache_all();
      }
    }
    else {
      if ($wildcard) {
        if ($cid == '*') {
          yottaa_flush_yottaa_cache_all();
        }
        else {
          $path = $cid . '(.*)';
          yottaa_flush_yottaa_cache_pages(array($path));
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

  function isEmpty() {
    return FALSE;
  }
}
