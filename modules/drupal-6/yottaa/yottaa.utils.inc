<?php
/**
 * @file
 *
 */
/**
 * Accepts provided http content, checks for a valid http response,
 * unchunks if needed, returns http content without headers on
 * success, FALSE on any errors.
 *
 * @param NULL $content
 * @return bool|string
 */
function yottaa_parse_http_response($content = NULL) {
  if (empty($content)) {
    return FALSE;
  }
  // split into array, headers and content.
  $hunks = explode("\r\n\r\n", trim($content));
  if (!is_array($hunks) or count($hunks) < 2) {
    return FALSE;
  }
  $header = $hunks[count($hunks) - 2];
  $body = $hunks[count($hunks) - 1];
  $headers = explode("\n", $header);
  unset($hunks);
  unset($header);
  if (!yottaa_validate_http_response($headers)) {
    return '{"error" : ' . trim($body) . '}';
    //return FALSE;
  }
  if (in_array('Transfer-Coding: chunked', $headers)) {
    return trim(yottaa_unchunk_http_response($body));
  }
  else {
    return trim($body);
  }
}

/**
 * Validate http responses by checking header.  Expects array of
 * headers as argument.  Returns boolean.
 *
 * @param NULL $headers
 * @return bool
 */
function yottaa_validate_http_response($headers = NULL) {
  if (!is_array($headers) or count($headers) < 1) {
    return FALSE;
  }
  switch (trim(strtolower($headers[0]))) {
    case 'http/1.0 100 ok':
    case 'http/1.0 200 ok':
    case 'http/1.1 100 ok':
    case 'http/1.1 200 ok':
      return TRUE;
      break;
  }
  return FALSE;
}

/**
 * Unchunk http content.  Returns unchunked content on success,
 * FALSE on any errors...  Borrows from code posted above by
 * jbr at ya-right dot com.
 *
 * @param NULL $str
 * @return bool|NULL|string
 */
function yottaa_unchunk_http_response($str = NULL) {
  if (!is_string($str) or strlen($str) < 1) {
    return FALSE;
  }
  $eol = "\r\n";
  $add = strlen($eol);
  $tmp = $str;
  $str = '';
  do {
    $tmp = ltrim($tmp);
    $pos = strpos($tmp, $eol);
    if ($pos === FALSE) {
      return FALSE;
    }
    $len = hexdec(substr($tmp, 0, $pos));
    if (!is_numeric($len) or $len < 0) {
      return FALSE;
    }
    $str .= substr($tmp, ($pos + $add), $len);
    $tmp = substr($tmp, ($len + $pos + $add));
    $check = trim($tmp);
  } while (!empty($check));
  unset($tmp);
  return $str;
}

/**
 * @param $url
 * @param $params
 * @param $method
 * @return string
 */
function yottaa_curl_post_async($url, $params, $method, $api_key) {
  foreach ($params as $key => &$val) {
    if (is_array($val)) $val = implode(',', $val);
    $post_params[] = $key . '=' . urlencode($val);
  }
  $post_string = implode('&', $post_params);

  $parts = parse_url($url);

  $fp = fsockopen("ssl://" . $parts['host'],
          isset($parts['port']) ? $parts['port'] : 443,
          $errno, $errstr, 30);

  // Data goes in the path for a GET request
  $parts['path'] .= '?' . $post_string;

  $out = $method . " " . $parts['path'] . " HTTP/1.1\r\n";
  $out .= "Host: " . $parts['host'] . "\r\n";
  $out .= "Content-Type: application/x-www-form-urlencoded\r\n";
  $out .= "Content-Length: 0\r\n";
  $out .= "YOTTAA-API-KEY: " . $api_key . "\r\n";
  $out .= "Connection: Close\r\n\r\n";

  fwrite($fp, $out);
  $result = "";
  while (!feof($fp)) {
    $result .= fgets($fp, 128);
  }
  fclose($fp);
  return $result;
}

/**
 * Creates a new Yottaa account.
 *
 * @param $full_name
 * @param $email
 * @param $phone
 * @param $site
 * @return mixed
 */
function yottaa_create_yottaa_account($full_name, $email, $phone, $site) {
  $user_id = '4d34f75b74b1553ba500007f';
  $api_key = '455df7500258012f663b12313d145ceb';
  list($first_name, $last_name) = explode(" ", $full_name);
  $output = yottaa_curl_post_async("https://api.yottaa.com/partners/" . $user_id . "/accounts", array("first_name" => $first_name, "last_name" => $last_name, "email" => $email, "phone" => $phone, "site" => $site), "POST", $api_key);
  return json_decode(yottaa_parse_http_response($output), TRUE);
}

/**
 * Checks status of the registered Yottaa optimizer.
 *
 * @return mixed
 */
function yottaa_check_yottaa_status() {
  $yottaa_user_id = variable_get('yottaa_user_id', '');
  $yottaa_api_key = variable_get('yottaa_api_key', '');
  $yottaa_site_id = variable_get('yottaa_site_id', '');
  $output = yottaa_curl_post_async("https://api.yottaa.com/sites/" . $yottaa_site_id, array("user_id" => $yottaa_user_id), "GET", $yottaa_api_key);
  return json_decode(yottaa_parse_http_response($output), TRUE);
}

/**
 * Resumes the registered Yottaa optimizer.
 *
 * @return mixed
 */
function yottaa_resume_yottaa_optimizer() {
  $yottaa_user_id = variable_get('yottaa_user_id', '');
  $yottaa_api_key = variable_get('yottaa_api_key', '');
  $yottaa_site_id = variable_get('yottaa_site_id', '');
  $output = yottaa_curl_post_async("https://api.yottaa.com/optimizers/" . $yottaa_site_id . "/resume", array("user_id" => $yottaa_user_id), "PUT", $yottaa_api_key);
  return json_decode(yottaa_parse_http_response($output), TRUE);
}

/**
 * Pauses the registered Yottaa optimizer.
 *
 * @return mixed
 */
function yottaa_pause_yottaa_optimizer() {
  $yottaa_user_id = variable_get('yottaa_user_id', '');
  $yottaa_api_key = variable_get('yottaa_api_key', '');
  $yottaa_site_id = variable_get('yottaa_site_id', '');
  $output = yottaa_curl_post_async("https://api.yottaa.com/optimizers/" . $yottaa_site_id . "/pause", array("user_id" => $yottaa_user_id), "PUT", $yottaa_api_key);
  return json_decode(yottaa_parse_http_response($output), TRUE);
}

/**
 * Fetches settings of the registered Yottaa optimizer.
 *
 * @return mixed
 */
function yottaa_retrieve_yottaa_settings() {
  $yottaa_user_id = variable_get('yottaa_user_id', '');
  $yottaa_api_key = variable_get('yottaa_api_key', '');
  $yottaa_site_id = variable_get('yottaa_site_id', '');
  $output = yottaa_curl_post_async("https://api.yottaa.com/sites/" . $yottaa_site_id . "/settings", array("user_id" => $yottaa_user_id), "GET", $yottaa_api_key);
  return yottaa_post_processing_settings(json_decode(yottaa_parse_http_response($output), TRUE));
}

/**
 * Asks the registered Yottaa optimizer to flush caches.
 *
 * @return mixed
 */
function yottaa_flush_yottaa_cache() {
  $yottaa_user_id = variable_get('yottaa_user_id', '');
  $yottaa_api_key = variable_get('yottaa_api_key', '');
  $yottaa_site_id = variable_get('yottaa_site_id', '');
  $output = yottaa_curl_post_async("https://api.yottaa.com/sites/" . $yottaa_site_id . "/flush_cache", array("user_id" => $yottaa_user_id), "PUT", $yottaa_api_key);
  return json_decode(yottaa_parse_http_response($output), TRUE);
}


/**
 * Post-process settings return from Yottaa service.
 *
 * @param $json_output
 * @return array
 */
function yottaa_post_processing_settings($json_output) {
  if (!isset($json_output["error"])) {

    $site_pages_key = "/";
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
                  if ($match["condition"] == $site_pages_key && $match["name"] == "URI" && $match["type"] == "0" && $match["operator"] == "CONTAIN") {
                    $only_cache_anonymous_users = $direction;
                  }
                  if ($match["name"] == "Request-Header" && $match["header_name"] == "Cookie" && $match["condition"] == "SESS" && $match["type"] == "0" && $match["operator"] == "NOT-CONTAIN") {
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
 * Automatically flushes cache for an updated node or a node whose comments have been updated,
 * created or deleted.
 *
 *
 * @param $node
 * @return void
 */
function yottaa_auto_flush_yottaa_cache($node) {
  $yottaa_auto_clear_cache = variable_get('yottaa_auto_clear_cache', 1);
  if ($yottaa_auto_clear_cache == 1) {
    $json_output = flush_yottaa_cache();
    // drupal_set_message(t('Cache flushed!'));
    if (isset($json_output["error"])) {
      $error = $json_output["error"];
      //form_set_error('', 'Error received from flushing Yottaa cache:' . json_encode($error));
      watchdog('yottaa', 'Error received when flushing Yottaa cache for node @node. @error', array('@node' => $node->nid, '@error' => json_encode($error)), WATCHDOG_ERROR);
    }
    watchdog('yottaa', 'Flushed Yottaa cache for node @node.', array('@node' => $node->nid), WATCHDOG_DEBUG);
  }
}

/**
 * Flushes all cached Yottaa pages.
 *
 * @return void
 */
function yottaa_flush_yottaa_cache_all() {
  $json_output = yottaa_flush_yottaa_cache();
  if (isset($json_output["error"])) {
    $error = $json_output["error"];
    watchdog('yottaa', 'Error received when flushing all Yottaa cache. @error', array('@error' => json_encode($error)), WATCHDOG_ERROR);
  }
  else {
    watchdog('yottaa', 'Flushed all Yottaa cache.', array(), WATCHDOG_DEBUG);
  }
}

/**
 * Flushed cached Yottaa pages with the given paths.
 *
 * @param $paths
 * @return void
 */
function yottaa_flush_yottaa_cache_pages($paths) {
  $json_output = yottaa_flush_yottaa_cache();
  if (isset($json_output["error"])) {
    $error = $json_output["error"];
    watchdog('yottaa', 'Error received when flushing Yottaa cache for paths @path_list. @error', array('@path_list' => implode(';', $paths),'@error' => json_encode($error)), WATCHDOG_ERROR);
  }
  else {
    watchdog('yottaa', 'Flushed Yottaa cache for paths @path_list.', array('@path_list' => implode(';', $paths)), WATCHDOG_DEBUG);
  }
}

