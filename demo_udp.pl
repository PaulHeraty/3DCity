#!/usr/bin/perl -w

use IO::File;
use IO::Socket::INET;

use strict;
use POSIX;
use Time::HiRes qw(usleep nanosleep);

my $CinemoPath = "/root/cinemox11/usr/bin/";
my $VideoPath =  "/root/";
my $CinemoDeltaX = 1920;
my $CinemoDeltaY = 0;

my @forked_pids = ();
my @Videos = ();

sub GetRand
{
   my ($m) = @_;
   return floor(rand()*$m);
}


sub PushVideo
{
   my ($name) = @_;
   push(@Videos, $name);
}

sub GetRandVideo()
{
  my $i = 0;
  my $sz = @Videos;
  $i = GetRand($sz);
  return $Videos[$i];
}


PushVideo("$VideoPath/Speed_1080.mp4");


my $cwd = getcwd();
print "In perl script ($cwd)\n";

sub do_test0
{
        open(STDIN, "<", "XXX");
}

my $time_to_quit = 0;
sub on_signal
{
        $time_to_quit = 1;
        print "### SIGNAL !\n";
#        do_test0();
        $SIG{INT}='DEFAULT';
}

$SIG{INT}=\&on_signal;


sub AddPid
{
   my ($pid,$cmd) = @_;
   my $idem_data = {};
   $idem_data->{pid} = $pid;
   $idem_data->{cmd} = $cmd;
   push(@forked_pids, $idem_data);
}

sub StartProcess
{
        my ($cmd) = @_;
        my $pid = fork();
        die "fork failed" unless defined $pid;
        print "PID: $pid\n" unless !($pid);
        AddPid($pid,  $cmd);
        if ($pid == 0){
                exec($cmd) or die "==FAILED exec($cmd)\n";
                exit(2);
        }
}

sub AddCinemo
{
        my ($sx, $sy, $px, $py, $file) = @_;
        my $app_path = $CinemoPath;
        chdir $app_path;
        #my $common = "$app_path/NmeCmdLine -a0 -r -ii -fc -sd ";
        my $common = "$app_path/NmeCmdLine -a0 -r -nc -sd ";
        if (1) {
                # Random start
                  $common .= "-t ".(GetRand(60)*500)." ";
        }
        my $cmd = "$common -x $sx -y $sy -xx $px -yy $py $file ";
        $cmd .= "";
        print "Start Cinemo $file\n";
        StartProcess($cmd);
        chdir $cwd;
}

sub AddCinemoAuto
{
        my ($sx, $sy, $px, $py) = @_;
        my $file = GetRandVideo();
        AddCinemo($sx, $sy, $px, $py, $file);
}


sub DoKill
{
        my ($p) = @_;
        print "Kill $p\n";
        my $res = waitpid($p, WNOHANG);
        if ($res == 0) {
                kill(2, $p);
                usleep(500);
        my $res2 = waitpid($p, WNOHANG);
                if ($res2 == 0) {
                        print "Kill 9 ! $p\n";
                        kill 9,$p;
                }
        }

}

sub WaitPids
{
        my ($k) = @_;
        my $check;
        my $cnt = 1;
        my $status;
        print "List Pid\n";
        foreach my $item (@forked_pids){
                $check = "/proc/$item->{pid}";
                if (!(-x $check)) {
                        $status = "DEAD";
                } else {
                        $status = "ALIVE";
                        if ($k) {
                                DoKill($item->{pid});
                                $status = "KILLED";

                        }
                }
                print " $cnt $item->{pid} $item->{cmd} $status\n";
                $cnt++;
        }
        print "List Pid - done\n";
}

if (0) {
        my $rx;
        my $ry;
        for ($rx=0; $rx<4; $rx++) {
                for ($ry=0; $ry<3; $ry++) {
                        AddCinemoAuto(960, 350, ($rx*960), ($ry*350));
                }
        }

}

my $CurrentCount = 0;
sub GoCinemo
{
if ($CurrentCount<16) {
   my $ax = floor($CurrentCount / 4);
   my $ay = $CurrentCount % 4;
   my $bx = 400;
   my $by = 250;
   my $cx = 80;
   my $cy = 30;
   AddCinemoAuto($bx, $by, $CinemoDeltaX + (($bx+$cx)*$ax), $CinemoDeltaY +(($by+$cy)*$ay));
   $CurrentCount++;
}
}

GoCinemo();
WaitPids(0);

if (1) {
# keyboard
  system "stty", '-icanon', 'eol', "\001";
  my $string;
  while (!$time_to_quit) {
       print "Wait keyboard - start \n";
       $string = getc(STDIN);
       print "Wait keyboard - stop \n";
       if ($string eq "?") {
       }
        elsif ($string eq "a") {
          GoCinemo();
          print " => $CurrentCount\n";
        }
        elsif ($string eq " ") {
               WaitPids(0);
       }  else {
               print ">>>> Got '$string'\n";
               $time_to_quit = 1;
       }
  }
  print "Exiting...\n";
  system 'stty', 'sane';
}

if (0) {
# Udp
  my $socket;
  my $string;
  my $recieved_data;
  $socket = new IO::Socket::INET (
        LocalPort => '8888',
        Proto => 'udp',
        ReuseAddr => 1,
  ) or die "Socket failed ! : $!\n";
  while (!$time_to_quit) {
        $socket->recv($recieved_data,1024);
        print "<$recieved_data>\n";
        if ($recieved_data eq "VideoCmd1") {
                GoCinemo();
        } elsif ($recieved_data eq "BYE") {
                $time_to_quit = 1;
        }
  }
  printf "exit";
}


WaitPids(0);
WaitPids(1);

print "Over...\n";
