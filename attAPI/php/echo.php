<?php
require_once('./lib/nusoap.php');

$JASPER_URI = 'http://api.jasperwireless.com/ws/schema';

$env = 'apitest'; # Apitest URL. See "Get WSDL Files" in the API documentation for Production URL.
$wsdlUrl = 'http://'.$env.'.jasperwireless.com/ws/schema/Echo.wsdl';
$licenseKey = '***REMOVED***';

$service = new nusoap_client($wsdlUrl, true /** wsdl **/);
$msg =
'<EchoRequest xmlns="'.$JASPER_URI.'">'.
'<messageId></messageId>'.
'<version></version>'.
'<licenseKey>'.$licenseKey.'</licenseKey>'.
'<value>Hello World</value>'.
'</EchoRequest>';
$result = $service->call('Echo', $msg);

if ($service->fault) {
  echo 'faultcode: ' . $service->faultcode . "\n";
  echo 'faultstring: ' . $service->faultstring . "\n";
  echo 'faultDetail: ' . $service->faultdetail . "\n";
  echo 'response: ' . $service->response;
  exit(0);
}

echo 'Request: ' . $service->request . "\n";
echo 'Response: ' . $service->response . "\n";
echo 'Call result: ' . $result['value'] . "\n";
?>
