<?php
require_once('./lib/nusoap.php');

$JASPER_URI = 'http://api.jasperwireless.com/ws/schema';

$env = 'apitest'; # Apitest URL. See "Get WSDL Files" in the API documentation for Production URL.
$wsdlUrl = 'http://'.$env.'.jasperwireless.com/ws/schema/Terminal.wsdl';
$licenseKey = '***REMOVED***';
$userName = '***REMOVED***';
$password = '***REMOVED***';

$service = new nusoap_client($wsdlUrl, true /** wsdl **/);
$service->setHeaders(
'<wsse:Security xmlns:wsse="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd">'.
'<wsse:UsernameToken>'.
'<wsse:Username>'.$userName.'</wsse:Username>'.
'<wsse:Password Type="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordText">'.$password.'</wsse:Password>'.
'</wsse:UsernameToken>'.
'</wsse:Security>'
);
$msg =
'<GetTerminalDetailsRequest xmlns="'.$JASPER_URI.'">'.
'<messageId></messageId>'.
'<version></version>'.
'<licenseKey>'.$licenseKey.'</licenseKey>'.
'<iccids>'.
'<iccid>'.'INSERT ICCID HERE'.'</iccid>'.
'<iccid>'.'INSERT ANOTHER ICCID HERE'.'</iccid>'.
'</iccids>'.
'</GetTerminalDetailsRequest>';
$result = $service->call('GetTerminalDetails', $msg);

if ($service->fault) {
  echo 'faultcode: ' . $service->faultcode . "\n";
  echo 'faultstring: ' . $service->faultstring . "\n";
  echo 'faultDetail: ' . $service->faultdetail . "\n";
  echo 'response: ' . $service->response;
  exit(0);
}

echo 'Request: ' . $service->request . "\n";
echo 'Response: ' . $service->response . "\n";
echo "Call result: \n";
var_dump($result['terminals']['terminal']);
echo "Status of the first terminal: " . $result['terminals']['terminal'][0]['status'] . "\n";
?>
