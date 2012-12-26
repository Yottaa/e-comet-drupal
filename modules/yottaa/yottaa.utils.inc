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
    $output = curl_post_async("https://api.yottaa.com/optimizers/" . $yottaa_site_id . "/settings", array("user_id" => $yottaa_user_id), "GET", $yottaa_api_key);
    return json_decode(parseHttpResponse($output), true);
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
