<?php
/**
 * Accepts provided http content, checks for a valid http response,
 * unchunks if needed, returns http content without headers on
 * success, false on any errors.
 *
 * @param null $content
 * @return bool|string
 */
function parseHttpResponse($content = null)
{
    if (empty($content)) {
        return false;
    }
    // split into array, headers and content.
    $hunks = explode("\r\n\r\n", trim($content));
    if (!is_array($hunks) or count($hunks) < 2) {
        return false;
    }
    $header = $hunks[count($hunks) - 2];
    $body = $hunks[count($hunks) - 1];
    $headers = explode("\n", $header);
    unset($hunks);
    unset($header);
    if (!validateHttpResponse($headers)) {
        return false;
    }
    if (in_array('Transfer-Coding: chunked', $headers)) {
        return trim(unchunkHttpResponse($body));
    } else {
        return trim($body);
    }
}

/**
 * Validate http responses by checking header.  Expects array of
 * headers as argument.  Returns boolean.
 *
 * @param null $headers
 * @return bool
 */
function validateHttpResponse($headers = null)
{
    if (!is_array($headers) or count($headers) < 1) {
        return false;
    }
    switch (trim(strtolower($headers[0]))) {
        case 'http/1.0 100 ok':
        case 'http/1.0 200 ok':
        case 'http/1.1 100 ok':
        case 'http/1.1 200 ok':
            return true;
            break;
    }
    return false;
}

/**
 * Unchunk http content.  Returns unchunked content on success,
 * false on any errors...  Borrows from code posted above by
 * jbr at ya-right dot com.
 *
 * @param null $str
 * @return bool|null|string
 */
function unchunkHttpResponse($str = null)
{
    if (!is_string($str) or strlen($str) < 1) {
        return false;
    }
    $eol = "\r\n";
    $add = strlen($eol);
    $tmp = $str;
    $str = '';
    do {
        $tmp = ltrim($tmp);
        $pos = strpos($tmp, $eol);
        if ($pos === false) {
            return false;
        }
        $len = hexdec(substr($tmp, 0, $pos));
        if (!is_numeric($len) or $len < 0) {
            return false;
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
function curl_post_async($url, $params, $method, $api_key)
{
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
function create_yottaa_account($full_name, $email, $phone, $site)
{
    $user_id = '4d34f75b74b1553ba500007f';
    $api_key = '455df7500258012f663b12313d145ceb';
    list($first_name, $last_name) = explode(" ", $full_name);
    $output = curl_post_async("https://api.yottaa.com/partners/" . $user_id . "/accounts", array("first_name" => $first_name, "last_name" => $last_name, "email" => $email, "phone" => $phone, "site" => $site), "POST", $api_key);
    return json_decode(parseHttpResponse($output), true);
}

/**
 * Checks status of the registered Yottaa optimizer.
 *
 * @return mixed
 */
function check_yottaa_status()
{
    $yottaa_user_id = variable_get('yottaa_user_id', '');
    $yottaa_api_key = variable_get('yottaa_api_key', '');
    $yottaa_site_id = variable_get('yottaa_site_id', '');
    $output = curl_post_async("https://api.yottaa.com/sites/" . $yottaa_site_id, array("user_id" => $yottaa_user_id), "GET", $yottaa_api_key);
    return json_decode(parseHttpResponse($output), true);
}

/**
 * Resumes the registered Yottaa optimizer.
 *
 * @return mixed
 */
function resume_yottaa_optimizer()
{
    $yottaa_user_id = variable_get('yottaa_user_id', '');
    $yottaa_api_key = variable_get('yottaa_api_key', '');
    $yottaa_site_id = variable_get('yottaa_site_id', '');
    $output = curl_post_async("https://api.yottaa.com/optimizers/" . $yottaa_site_id . "/resume", array("user_id" => $yottaa_user_id), "PUT", $yottaa_api_key);
    return json_decode(parseHttpResponse($output), true);
}

/**
 * Pauses the registered Yottaa optimizer.
 *
 * @return mixed
 */
function pause_yottaa_optimizer()
{
    $yottaa_user_id = variable_get('yottaa_user_id', '');
    $yottaa_api_key = variable_get('yottaa_api_key', '');
    $yottaa_site_id = variable_get('yottaa_site_id', '');
    $output = curl_post_async("https://api.yottaa.com/optimizers/" . $yottaa_site_id . "/pause", array("user_id" => $yottaa_user_id), "PUT", $yottaa_api_key);
    return json_decode(parseHttpResponse($output), true);
}

/**
 * Fetches settings of the registered Yottaa optimizer.
 *
 * @return mixed
 */
function retrieve_yottaa_settings()
{
    $yottaa_user_id = variable_get('yottaa_user_id', '');
    $yottaa_api_key = variable_get('yottaa_api_key', '');
    $yottaa_site_id = variable_get('yottaa_site_id', '');
    $output = curl_post_async("https://api.yottaa.com/sites/" . $yottaa_site_id . "/settings", array("user_id" => $yottaa_user_id), "GET", $yottaa_api_key);
    return post_processing_settings(json_decode(parseHttpResponse($output), true));
}

/**
 * Asks the registered Yottaa optimizer to flush caches.
 *
 * @return mixed
 */
function flush_yottaa_cache()
{
    $yottaa_user_id = variable_get('yottaa_user_id', '');
    $yottaa_api_key = variable_get('yottaa_api_key', '');
    $yottaa_site_id = variable_get('yottaa_site_id', '');
    $output = curl_post_async("https://api.yottaa.com/optimizers/" . $yottaa_site_id . "/flush_cache", array("user_id" => $yottaa_user_id), "PUT", $yottaa_api_key);
    return json_decode(parseHttpResponse($output), true);
}


/**
 * Post-process settings return from Yottaa service.
 *
 * @param $json_output
 * @return array
 */
function post_processing_settings($json_output)
{
    if (!isset($json_output["error"])) {

        $prod_key = "product";
        $cat_key = "category";
        $site_pages_key = "(.*)";
        $edit_pages_key = "/edit";

        $prod_caching = 'unknown';
        $cat_caching = 'unknown';
        $ajax_cart_caching = 'unknown';
        $admin_caching = 'unknown';

        $site_pages_caching = 'unknown';
        $edit_pages_caching = 'unknown';
        $exclusions = '';

        if (isset($json_output["defaultActions"]) && isset($json_output["defaultActions"]["resourceActions"]) && isset($json_output["defaultActions"]["resourceActions"]["htmlCache"])) {
            $html_cachings = $json_output["defaultActions"]["resourceActions"]["htmlCache"];
            foreach ($html_cachings as &$html_caching) {
                if (isset($html_caching["filters"])) {
                    $filters = $html_caching["filters"];
                    foreach ($filters as &$filter) {
                        if (isset($filter["match"])) {
                            $direction = $filter["direction"] == 1 ? "included" : "excluded";
                            $matches = $filter["match"];
                            foreach ($matches as &$match) {
                                if (isset($match["condition"])) {
                                    if (!(strpos($match["condition"], $prod_key) === false)) {
                                        $prod_caching = $direction;
                                    }
                                    if (!(strpos($match["condition"], $cat_key) === false)) {
                                        $cat_caching = $direction;
                                    }
                                    if (!(strpos($match["condition"], 'ajax_cart') === false)) {
                                        $ajax_cart_caching = $direction;
                                    }
                                    if (!(strpos($match["condition"], 'admin.mvc') === false)) {
                                        $admin_caching = $direction;
                                    }
                                    if (!(strpos($match["condition"], $site_pages_key) === false)) {
                                        $site_pages_caching = $direction;
                                    }
                                    if (!(strpos($match["condition"], $edit_pages_key) === false)) {
                                        $edit_pages_caching = $direction;
                                    }
                                }
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

        return array('prod_caching' => $prod_caching,
                     'cat_caching' => $cat_caching,
                     'ajax_cart_caching' => $ajax_cart_caching,
                     'admin_caching' => $admin_caching,
                     'site_pages_caching' => $site_pages_caching,
                     'edit_pages_caching' => $edit_pages_caching,
                     'exclusions' => $exclusions);
    } else {
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
function auto_flush_yottaa_cache($node)
{
    $yottaa_auto_clear_cache = variable_get('yottaa_auto_clear_cache', 1);
    if ($yottaa_auto_clear_cache == 1) {
        $json_output = flush_yottaa_cache();
        // drupal_set_message(t('Cache flushed!'));
        if (isset($json_output["error"])) {
            $error = $json_output["error"];
            form_set_error('', 'Error received from flushing Yottaa cache:' . json_encode($error));
        }
    }
}
