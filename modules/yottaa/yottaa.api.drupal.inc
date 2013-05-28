<?php

/**
 * @file
 * Yottaa Drupal API Class.
 */

class YottaaDrupalAPI extends YottaaAPI {

  public $api_key_varname = 'yottaa_api_key';
  public $user_id_varname = 'yottaa_user_id';
  public $site_id_varname = 'yottaa_site_id';
  public $auto_clear_cache_varname = 'yottaa_auto_clear_cache';
  public $enable_logging_varname = 'yottaa_enable_logging';

  public function __construct() {
    $key = variable_get($this->api_key_varname, '');
    $uid = variable_get($this->user_id_varname, '');
    $sid = variable_get($this->site_id_varname, '');
    parent::__construct($key, $uid, $sid);
  }

  /**
   * @return array
   */
  public function getParameters() {
      return array("api_key" => variable_get($this->api_key_varname, ""),
                   "user_id" => variable_get($this->user_id_varname, ""),
                   "site_id" => variable_get($this->site_id_varname, ""),
      );
  }

  /**
   * @param $key
   * @param $uid
   * @param $sid
   * @return void
   */
  public function updateParameters($key, $uid, $sid) {
    variable_set($this->user_id_varname, $uid);
    variable_set($this->api_key_varname, $key);
    variable_set($this->site_id_varname, $sid);
    parent::updateParameters($key, $uid, $sid);
  }

  /**
   * @return void
   */
  public function deleteParameters() {
    variable_del($this->user_id_varname);
    variable_del($this->api_key_varname);
    variable_del($this->site_id_varname);
    parent::deleteParameters();
  }

  /**
   * @return
   */
  public function getAutoClearCacheParameter() {
    return variable_get($this->auto_clear_cache_varname, 1);
  }

  /**
   * @param $enabled
   * @return void
   */
  public function setAutoClearCacheParameter($enabled) {
    variable_set($this->auto_clear_cache_varname, $enabled);
  }

  /**
   * @return
   */
  public function getEnableLoggingParameter() {
    return variable_get($this->enable_logging_varname, 1);
  }

  /**
   * @param $enabled
   * @return void
   */
  public function setEnableLoggingParameter($enabled) {
    variable_set($this->enable_logging_varname, $enabled);
  }

  /**
   * Post-process settings return from Yottaa service.
   *
   * @param $json_output
   * @return array
   */
  protected function postProcessingSettings($json_output) {
    if (!isset($json_output["error"])) {

      //$site_pages_key = "/";
      $full_pages_key = "(.*)";
      $configure_pages_key1 = "admin/";
      $configure_pages_key2 = "admin%252F";
      $edit_pages_key = "/edit";

      $site_pages_caching = 'unknown';
      $only_cache_anonymous_users = 'unknown';
      $edit_pages_caching = 'unknown';
      $configure_pages_caching = 'unknown';
      $configure_pages_caching1 = 'unknown';
      $configure_pages_caching2 = 'unknown';

      $exclusions = '';

      $excluded_sess_cookie = 'unknown';

      if (isset($json_output["defaultActions"]) && isset($json_output["defaultActions"]["resourceActions"]) && isset($json_output["defaultActions"]["resourceActions"]["htmlCache"])) {
        $html_cachings = $json_output["defaultActions"]["resourceActions"]["htmlCache"];
          foreach ($html_cachings as &$html_caching) {
            if ($html_caching["enabled"]) {
              $site_pages_caching = 'included';
          }
          if (isset($html_caching["filters"])) {
            $filters = $html_caching["filters"];
            foreach ($filters as &$filter) {
              if (isset($filter["match"])) {
                $direction = $filter["direction"] == 1 ? "included" : "excluded";
                $matches = $filter["match"];
                foreach ($matches as &$match) {
                  if (isset($match["condition"])) {
                    /*
                    if ($match["condition"] == $site_pages_key && $match["name"] == "URI" && $match["type"] == "0" && $match["operator"] == "CONTAIN") {
                      $only_cache_anonymous_users = $direction;
                    }
                    if ($match["name"] == "Request-Header" && $match["header_name"] == "Cookie" && $match["condition"] == "SESS" && $match["type"] == "0" && $match["operator"] == "NOT-CONTAIN") {
                      $excluded_sess_cookie = "set";
                    }
                    */
                    if ($match["condition"] == $full_pages_key && $match["name"] == "URI" && $match["type"] == "0" && $match["operator"] == "REGEX") {
                      $only_cache_anonymous_users = $direction;
                    }
                    if ($match["name"] == "Request-Header" && $match["header_name"] == "Cookie" && $match["condition"] == "SESS" && $match["type"] == "0" && $match["operator"] == "CONTAIN") {
                      $excluded_sess_cookie = "set";
                    }
                    if ($match["condition"] == $edit_pages_key && $match["name"] == "URI" && $match["type"] == "0" && $match["operator"] == "CONTAIN") {
                      $edit_pages_caching = $direction;
                    }
                    if ($match["condition"] == $configure_pages_key1 && $match["name"] == "URI" && $match["type"] == "0" && $match["operator"] == "CONTAIN") {
                      $configure_pages_caching1 = $direction;
                    }
                    if ($match["condition"] == $configure_pages_key2 && $match["name"] == "URI" && $match["type"] == "0" && $match["operator"] == "CONTAIN") {
                      $configure_pages_caching2 = $direction;
                    }
                  }
                }
                if ($configure_pages_caching1 == "excluded" && $configure_pages_caching2 == "excluded") {
                  $configure_pages_caching = "excluded";
                }
                if ($only_cache_anonymous_users == "unknown" || $excluded_sess_cookie != "set") {
                  $only_cache_anonymous_users = "unknown";
                  $excluded_sess_cookie = "unknown";
                }
              }
            }
          }
        }
      }

      if (isset($json_output["defaultActions"]) && isset($json_output["defaultActions"]["filters"])) {
        $filters = $json_output["defaultActions"]["filters"];
        foreach ($filters as &$filter) {
          if (isset($filter["match"])) {
            if ($filter["direction"] == 0) {
              $matches = $filter["match"];
              foreach ($matches as &$match) {
                if (isset($match["condition"])) {
                  if ($exclusions != '') {
                    $exclusions = $exclusions . ' ; ';
                  }
                  $exclusions = $exclusions . $match["condition"];
                }
              }
            }
          }
        }
      }

      return array('site_pages_caching' => $site_pages_caching,
             'edit_pages_caching' => $edit_pages_caching,
             'configure_pages_caching' => $configure_pages_caching,
             'only_cache_anonymous_users' => $only_cache_anonymous_users,
             'exclusions' => $exclusions);
    }
    else {
      return $json_output;
    }
  }

  /**
   * Logs a message.
   *
   * @param $message
   * @return void
   */
  public function log($message) {
    if ($this->getEnableLoggingParameter() == 1) {
      if (is_array($message) || is_object($message)) {
        watchdog('yottaa', print_r($message, TRUE));
      }
      else {
        watchdog('yottaa', $message);
      }
    }
  }
}
/**
 * Wrapper function for Yottaa Drupal API Class.
 */
function yottaa_api_drupal() {
  static $api;
  if (!isset($api)) {
    $api = new YottaaDrupalAPI();
  }
  return $api;
}