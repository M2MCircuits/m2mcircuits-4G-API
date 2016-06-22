# Send an SMS using Jasper API from AT&T
# To run, you need to replace $licenseKey, $userName, $password, and $recipient values

# Usage: $ perl sendSMS.pl <drone message> <message id>

#!perl -w

use strict;
use Data::Dumper;
use SOAP::Lite +trace => ['debug'];

my $JASPER_URI = 'http://api.jasperwireless.com/ws/schema';
my $WSSE_URI = 'http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd';
my $PASSWORD_TYPE = 'http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordText';

my $env = 'api';
my $url = 'https://'.$env.'.jasperwireless.com/ws/service/sms';
my $soapAction = 'http://api.jasperwireless.com/ws/service/sms/SendSMS';
my $licenseKey = '<API_LICENSE_KEY>';
my $userName = '<USERNAME>';
my $password = '<PASSWORD>';
my $recipient = '<SIM_ICCID>';
my $messageinput = $ARGV[0];
my $messageid = $ARGV[1];

my $service = SOAP::Lite->proxy($url)->uri($JASPER_URI)->autotype(0)->readable(1);
$service->on_fault(sub {
    my ($soap,$response) = @_;
    die('The following SOAP fault occurred:', "\n",
	' faultcode: ', $response->faultcode(), "\n",
	' faultstring: ', $response->faultstring(), "\n",
	' faultdetail: ', Dumper($response->faultdetail()), "\n")
		   });

$service->on_action(sub{return $soapAction;});
my $result = $service->call('SendSMSRequest' => (
  SOAP::Header->uri($WSSE_URI)->name('wsse:Security' =>
      \SOAP::Data->name('wsse:UsernameToken'=>
          \SOAP::Data->value(
	      SOAP::Data->name('wsse:Username' => $userName),
	      SOAP::Data->name('wsse:Password' => $password)->
		   attr({Type => $PASSWORD_TYPE})))),
  SOAP::Data->name('messageId' => $messageid),
  SOAP::Data->name('version' => '5.0.1'),
  SOAP::Data->name('licenseKey' => $licenseKey),
  SOAP::Data->name('sentToIccid' => $recipient),
  SOAP::Data->name('messageText' => $messageinput),
  SOAP::Data->name('tpvp' => 0)
));
print "Call result:  SUCCESSFUL\n";
