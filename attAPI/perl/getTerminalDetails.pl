#!perl -w
use strict;
use Data::Dumper;
use SOAP::Lite;

my $JASPER_URI = 'http://api.jasperwireless.com/ws/schema';
my $WSSE_URI = 'http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd';
my $PASSWORD_TYPE = 'http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordText';

my $env = 'apitest'; # Apitest URL. See "Get WSDL Files" in the API documentation for Production URL.
my $url = 'https://'.$env.'.jasperwireless.com/ws/service/terminal';
my $soapAction = 'http://api.jasperwireless.com/ws/service/terminal/GetTerminalDetails';
my $licenseKey = '***REMOVED***';
my $userName = '***REMOVED***';
my $password = '***REMOVED***';

my $service = SOAP::Lite->proxy($url)->uri($JASPER_URI)->autotype(0)->readable(1);
$service->on_fault(sub {
    my ($soap,$response) = @_;
    die('The following SOAP fault occurred:', "\n",
	' faultcode: ', $response->faultcode(), "\n",
	' faultstring: ', $response->faultstring(), "\n",
	' faultdetail: ', Dumper($response->faultdetail()), "\n")
    });

$service->on_action(sub{return $soapAction;});
my $result = $service->call('GetTerminalDetailsRequest' => (
  SOAP::Header->uri($WSSE_URI)->name('wsse:Security' =>
      \SOAP::Data->name('wsse:UsernameToken'=>
          \SOAP::Data->value(
	      SOAP::Data->name('wsse:Username' => $userName),
	      SOAP::Data->name('wsse:Password' => $password)->
		   attr({Type => $PASSWORD_TYPE})))),
 SOAP::Data->name('messageId' => ''),
  SOAP::Data->name('version' => ''),
  SOAP::Data->name('licenseKey' => $licenseKey),
  SOAP::Data->name('iccids' =>
      \SOAP::Data->value(
          SOAP::Data->name('iccid' => '<INSERT ICCID HERE>'),
          SOAP::Data->name('iccid' => '<INSERT ANOTHER ICCID HERE>')))
));

print "Call result:\n" . Dumper($result->valueof('//terminals/terminal')) . "\n";
print "Status of the first terminal: " . $result->valueof('//terminals/[1]/status') . "\n";