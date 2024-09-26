#!/usr/bin/perl -w

# Script to monitor the status of the online monitoring servers, and
# to start/restart/stop them, individually or all at once.
#
# Normal operation is checking every $interval milliseconds (default=3min)
#
# Left-Click on a subsystem button will check immediate check.
# Right-click will dump on starting shell window the machine:port:pid for this
# subsystem.
#
# L. Aphecetche (aphecetc@in2p3.fr)

use Tk;
use Tk::FileSelect;
use Time::localtime;
use DirHandle;
use Tk::Image;
use File::Basename;

use strict;

# Check frequency. Don't set it too high, as it takes some time to check
# all the servers (about 15 seconds at least).

my $interval = 300000; # ms => 5 min

my %monitors;

my $startall;
my $restartall;
my $stopall;

my %subsysmonitors;

my %excludeall = (
    "bbc" => 1,
    "ll1" => 1,
    "localpol" => 1,
    "sepd" => 1,
    "spin" => 1,
    "tpot" => 1,
    "zdc" => 1,
    );
#_____________________________________________________________________________
sub info {
    my $name = shift;
    print "$name monitor is supposed to run on ",$monitors{$name}->{"machine"},
	":",$monitors{$name}->{"number"}," pid=",
	$monitors{$name}->{"pid"},"\n";
}

#_____________________________________________________________________________
sub send_monitor_command {

    my $m = shift;
    my $swhat = shift;
    my $what = lc($swhat);

    if ( $what ne "start" && $what ne "stop" && $what ne "restart" ) {
	print "send_monitor_command : command $swhat unknown ! Should be start, restart or stop !\n";
	return;
    }

    my $cmd;

    if ( $m eq "all" ) {
	$cmd = "$ENV{'ONLMON_RUNDIR'}/submit_all.sh $what";
	if ( $what eq "restart" )
	{
	    system("restart_monitor_pool.sh");
	    system("sleep 5");
	}
	elsif ( $what eq "stop" )
	{
	    system("ssh -x etpool /etc/stop_et.sh");
	    system("sleep 20");
	}
	elsif ( $what eq "start" )
	{
	    system("ssh -x etpool /etc/start_et.csh");
	    system("sleep 10");
	}
    }
    else {
	my $va = $monitors{$m}->{"machine"};
	my $number = $monitors{$m}->{"number"};
	#    $cmd = "ssh -x $va \$ONLMON_MAIN/bin/monserver.csh $what $number";
	$cmd = "ssh -x $va \$ONLMON_SERVERWATCHER/monserver.sh $what $number";
    }
    print "send_monitor_command : $cmd\n";
    system($cmd);
    update($m);

    if ( $m eq "all" ) {
	$stopall->configure(-state=> ($what eq "stop" ? "disabled" : "active"));
	$startall->configure(-state=> ($what eq "stop" ? "active" : "disabled"));
	$restartall->configure(-state=> ($what eq "stop" ? "disabled" : "active"));
    }
}



sub send_subsys_monitor_command {

    my $m = shift;
    my $swhat = shift;

    my $what = lc($swhat);

    if ( $what ne "start" && $what ne "stop" && $what ne "restart" ) {
	print "send_monitor_command : command $swhat unknown ! Should be start, restart or stop !\n";
	return;
    }

    my $array_ref = $subsysmonitors{$m}->{"monitors"};
    foreach my $moni (sort @$array_ref)
    {
	print "send_monitor_command : $moni $swhat\n";
	send_monitor_command($moni,$swhat);
    }
}

sub send_monitor_command_to_all {

    my $swhat = shift;
    my $what = lc($swhat);

    if ( $what ne "start" && $what ne "stop" && $what ne "restart" ) {
	print "send_monitor_command : command $swhat unknown ! Should be start, restart or stop !\n";
	return;
    }

    foreach my $moni (sort keys %monitors)
    {
	send_monitor_command($moni,$swhat);
    }

}
#_____________________________________________________________________________
sub checkMonitor {

    my $m = shift;

    if ( $m eq "all" ) {
	foreach my $monitor ( sort keys %monitors ) {
	    checkMonitor($monitor);
	}
	return;
    }

    my $va = $monitors{$m}->{"machine"};
    my $number = $monitors{$m}->{"number"};
    #  my $cmd = "ssh -x phnxrc\@$va \$ONLMON_MAIN/bin/monserver.sh status $number";
    my $cmd = "ssh -x phnxrc\@$va \$ONLMON_SERVERWATCHER/monserver.sh status $number";

    my $bad=1;

    print ctime()." Checking $m on $va ...";

    open(STATUS,"$cmd |");
    my @result;
    while (<STATUS>) {
	push @result,$_;
	print "result: $_"; 
    }  
    close STATUS;
    my $nres = scalar @result;
    if ( scalar @result == 2 ) 
    {
	$_=$result[1];
	if ( /$va/ ) {
	    my @s = split " ";
	    $monitors{$m}->{"pid"} = $s[0];
	    $bad=0;
	}
    }

    if ( $bad != 0 ) {
	$monitors{$m}->{"nameButton"}->configure(-background=>'red');
	$monitors{$m}->{"startButton"}->configure(-state=>'active');
	$monitors{$m}->{"restartButton"}->configure(-state=>'active');
	$monitors{$m}->{"stopButton"}->configure(-state=>'disabled');
    }
    else {
	$monitors{$m}->{"nameButton"}->configure(-background=>'green');
	$monitors{$m}->{"startButton"}->configure(-state=>'disabled');
	$monitors{$m}->{"restartButton"}->configure(-state=>'active');
	$monitors{$m}->{"stopButton"}->configure(-state=>'active');
    }

    print " ".ctime()." done\n";
}

#_____________________________________________________________________________
sub getMonitors() {

    my $d = new DirHandle "$ENV{'ONLMON_SERVERWATCHER'}";

    if (defined $d) {
	while (defined($_ = $d->read)) {
	    if (/monitorserver/ && /cmd/ && $_ !~ /~/) {
		my $cmdfile = "$ENV{'ONLMON_SERVERWATCHER'}/$_";
		my ($va,$monitor,$number)=decode($cmdfile);
		decode_new($cmdfile);
	    }
	}
	undef $d;
    }
}

#_____________________________________________________________________________
sub decode_new {

    my $cmdfile = shift;
    
    my @result = split /\./,basename($cmdfile);

    my $va = $result[0];
    my $number = $result[2];

    open FILE, $cmdfile or die "Cannot open $cmdfile : $!";

    
    while (<FILE>) {
	chomp;
	if ( /^\/\// && /monitor:/ ) {
	    my $line = $_;
	    my $pos = index($line,"monitor:");
	    my $monitor = substr($line,$pos+length("monitor: "));
	    $monitors{$monitor} = {};
	    $monitors{$monitor}->{"machine"}=$va;
	    $monitors{$monitor}->{"number"}=$number;
	    $monitors{$monitor}->{"pid"}=0;
	    my @sp1 = split / /,$monitor;
	    if (! exists $excludeall{$monitor})
	    {
		push@{$subsysmonitors{$sp1[0]}->{"monitors"}},$monitor;
	    }
	}
    }

    close FILE;
    # print "%subsysmonitors\n";
    # foreach my $m (sort keys %subsysmonitors)
    # {
    #     print "looking at $m\n";
    #     my $array_ref = $subsysmonitors{$m}->{"monitors"};
    #     foreach my $node (sort @$array_ref)
    #     {#
    #	  print "key: $node\n";
    #      }
    #  }
}

#_____________________________________________________________________________
sub decode {

    my $cmdfile = shift;
    
    my @result = split /\./,$cmdfile;

    my $va = $result[0];
    my $number = $result[2];

    open FILE, $cmdfile or die "Cannot open $cmdfile : $!";

    my $line = <FILE>;

    close FILE;

    my $pos = index($line,"run_");

    $line = substr($line,$pos+length("run_"));

    @result = split /\./,$line;

    my $monitor = $result[0];

    return ($va,$monitor,$number);
}

#_____________________________________________________________________________
#_____________________________________________________________________________
#_____________________________________________________________________________

getMonitors();

my $mw = MainWindow->new;
my $column=0;
my $row=0;
my $ncolumns=10;

foreach my $m ( sort keys %monitors ) {
    my $name = $monitors{$m}->{"nameButton"} = 
	$mw->Button(-text => uc($m),
		    -relief=>'flat',
		    -width=>10);
    $name->bind('<Button-3>' => sub { info($m); } );
    $name->bind('<Button-1>' => sub { checkMonitor($m); } );
    $name->grid(-column=>$column,-row=>$row);
    $name->configure(-background=>'black');

    my $machine = $monitors{$m}->{"machineLabel"} =
	$mw->Label(-text=> $monitors{$m}->{"machine"});
    $machine->grid(-column=>$column,-row=>$row+1);

    my $start = $monitors{$m}->{"startButton"} = 
	$mw->Button(-text => "Start",
		    -relief => "raised", -width=>5);
    $start->grid(-column=>$column+1,-row=>$row);
    $start->configure(-state=>'disabled', 
		      -command=> sub { send_monitor_command($m,"start") } );

    my $restart = $monitors{$m}->{"restartButton"} = 
	$mw->Button(-text => "Restart",
		    -relief => "raised", -width=>5);
    $restart->grid(-column=>$column+1,-row=>$row+1);
    $restart->configure(-state=>'active', 
			-command=> sub { send_monitor_command($m,"restart") } );

    my $stop = $monitors{$m}->{"stopButton"} = 
	$mw->Button(-text => "Stop",
		    -relief => "raised", -width=>5);
    $stop->grid(-column=>$column+1,-row=>$row+2);  
    $stop->configure(-command=>sub { send_monitor_command($m,"stop") } );

    $column+=2;

    if ( $column/2 >= $ncolumns ) {
	$column=0;
	$row+=3;
    }

}

foreach my $m ( sort keys %subsysmonitors ) {
    my $button_label = sprintf("All %s",$m);
    my $name = $subsysmonitors{$m}->{"nameButton"} =
	$mw->Button(-text => uc($button_label),
		    -relief=>'flat',
		    -width=>10);
    #  $name->bind('<Button-3>' => sub { info($m); } );
    #  $name->bind('<Button-1>' => sub { checkMonitor($m); } );
    $name->grid(-column=>$column,-row=>$row);
    $name->configure(-background=>'cyan');

    my $start = $subsysmonitors{$m}->{"startButton"} =
	$mw->Button(-text => "Start All",
		    -relief => "raised", -width=>5);
    $start->grid(-column=>$column+1,-row=>$row);
    $start->configure(-state=>'active',
		      -command=> sub { send_subsys_monitor_command($m,"start") } );

    my $restart = $subsysmonitors{$m}->{"restartButton"} =
	$mw->Button(-text => "Restart All",
		    -relief => "raised", -width=>5);
    $restart->grid(-column=>$column+1,-row=>$row+1);
    $restart->configure(-state=>'active',
			-command=> sub { send_subsys_monitor_command($m,"restart") } );

    my $stop = $subsysmonitors{$m}->{"stopButton"} =
	$mw->Button(-text => "Stop All",
		    -relief => "raised", -width=>5);
    $stop->grid(-column=>$column+1,-row=>$row+2);
    $stop->configure(-state=>'active', -command=>sub { send_subsys_monitor_command($m,"stop") } );

    $column+=2;

    if ( $column/2 >= $ncolumns ) {
	$column=0;
	$row+=3;
    }

}


$column = 18;
my $allbutton = $mw->Button(-text => "Seriously ALL",
			    -relief=>'flat',
			    -width=>10);
$allbutton->grid(-column=>$column,-row=>$row);
$allbutton->configure(-background=>'orange');

$stopall = $mw->Button(-text=>'Stop All',
		       -command=> sub { send_monitor_command_to_all("stop") });

$stopall->grid(-column=>$column+1,-row=>$row);
$stopall->configure(-state=>'active',-width=>7);
$row++;
$startall = $mw->Button(-text=>'Start All',
			-command=> sub { send_monitor_command_to_all("start") });
$startall->grid(-column=>$column+1,-row=>$row);
$startall->configure(-state=>'active',-width=>7);
$row++;
$restartall = $mw->Button(-text=>'Restart All',
			  -command=> sub { send_monitor_command_to_all("restart") });
$restartall->grid(-column=>$column+1,-row=>$row);
$restartall->configure(-state=>'active',-width=>7);

my $logoimg = $mw->Photo('logo',-file=>"$ENV{'ONLMON_SERVERWATCHER'}/sphenix-logo_transparent_small.gif");
my $logo = $mw->Label('-image'=>'logo');
$row+=$ncolumns;
$logo->grid(-column=>0,-row=>$row,-columnspan=>2);

my $quitButton = $mw->Label(-text => "Quit", -relief => 'raised');
$quitButton->grid(-column=>2,-row=>$row);
$quitButton->bind('<Button-1>' => sub {$mw->destroy});
$quitButton->configure(-width=>5);


my $date = $mw->Label(-text => "", -relief => 'sunk');
$date->grid(-column=>(2*$ncolumns-2),-row=>$row,-columnspan=>2,-sticky=>'ew');

$mw->repeat($interval,\&update);

print "\n\n";

&update("all");

MainLoop;

#_____________________________________________________________________________
sub update() {

    my $m = shift;

    if (defined($m)) {
	checkMonitor($m);
    }
    else {
	checkMonitor("all");
    }
    $date->configure(-text => "Last check : " . ctime());
}
