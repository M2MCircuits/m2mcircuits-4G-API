#!perl -w
use strict;
use Data::Dumper;
use SOAP::Lite;

#my keyword for local variables, $varname = value
my $JASPER_URI = 'http://api.jasperwireless.com/ws/schema';
my $WSSE_URI = 'http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd';
my $PASSWORD_TYPE = 'http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordText';

my $env = 'apitest'; # Apitest URL. See "Get WSDL Files" in the API documentation for Production URL.
my $url = 'https://'.$env.'.jasperwireless.com/ws/service/echo';
my $soapAction = 'http://api.jasperwireless.com/ws/service/echo/Echo';
my $licenseKey = '<PUT LICENSE KEY HERE>';

#"->" is an infix dereference operator like in C
my $service = SOAP::Lite
->proxy($url)
->uri($JASPER_URI) #default_ns($JASPER_URI)
->autotype(0)
->readable(1);
#proxy: The proxy is the server or endpoint to which the client is going to connect. This method allows the setting of the endpoint, 
#	along with any extra information that the transport object may need when communicating the request.
#uri: DEPRECATED. The URI associated with this accessor on a client object is the service-specifier for the request, often encoded 
#	for HTTP-based requests as the SOAPAction header. While the names may seem confusing, this method doesn't specify 
#	the endpoint itself. In most circumstances, the uri refers to the namespace used for the request.
#autotype: By default, the serializer tries to automatically deduce types for the data being sent in a message. 
#	Setting a false value with this method disables the behavior.
#readable: When this is used to set a true value for this property, the generated XML sent to the endpoint has extra characters 
#	(spaces and new lines) added in to make the XML itself more readable to human eyes (presumably for debugging). 
#	The default is to not send any additional characters.

#error handling
$service->on_fault(sub {
    my ($soap,$response) = @_;
    die('The following SOAP fault occurred:', "\n",
	' faultcode: ', $response->faultcode(), "\n",
	' faultstring: ', $response->faultstring(), "\n",
	' faultdetail: ', Dumper($response->faultdetail()), "\n")
    });

#SOAP echo logic
$service->on_action(sub{return $soapAction;});
#on_action: Triggered when the transport object sets up the SOAPAction header for an HTTP-based call. 
#	The default is to set the header to the string, uri#method, in which URI is the value set by the uri method 
#	described earlier, and method is the name of the method being called. When called, the routine referenced 
#	(or the closure, if specified as in the example) is given two arguments, uri and method, in that order.
my $result = $service->call('EchoRequest' => (
  SOAP::Data->name('messageId' => ''),
  SOAP::Data->name('version' => ''),
  SOAP::Data->name('licenseKey' => $licenseKey),
  SOAP::Data->name('value' => 'Hello World')
));
#call: The SOAP::Lite client objects can manage remote calls with auto-dispatching using some of Perl's more elaborate features. 
#	call is used when the application wants a greater degree of control over the details of the call itself. The method may 
#	be built up from a SOAP::Data object, so as to allow full control over the namespace associated with the tag, as well as 
#	other attributes like encoding. This is also important for calling methods that contain characters not allowable in Perl 
#	function names, such as A.B.C.
print "Call result: " . $result->valueof('//value') . "\n";
