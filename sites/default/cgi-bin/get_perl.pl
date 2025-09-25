$buffer = $ENV{'QUERY_STRING'}; 

@pairs = split(/&/, $buffer); 
foreach $pair (@pairs)  
{ 
    ($name, $value) = split(/=/, $pair); 
    $value =~ tr/+/ /; 
    $value =~ s/%([a-fA-F0-9] [a-fA-F0-9])/pack("C", hex($1))/eg; 
    $value =~ s/~!/ ~!/g; 
    $FORM{$name} = $value;
} 
  
$Location = $FORM{'cbvalue'}; 
  
print "<html>"; 
print "<head>"; 
print "<title>Perl GET</title>"; 
print "</head>"; 
print "<body>";
print "<h1>Perl CGI</h1>";
print "<h3>You selected $Location<br>"; 
print "</body>"; 
print "</html>"; 
