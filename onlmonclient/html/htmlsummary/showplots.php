
<?php

// http://logbook.phenix.bnl.gov/OnlMon/mon.cgi?runnumber=327126&runtype=calibdata
// https://www.phenix.bnl.gov/WWW/run/10/online_monitoring/200GeV/mon.cgi?runnumber=310026&runtype=eventdata

$ncolumns  = 8;
$at_column = 0;
$scale   = "160";

$topdir     = $_POST[runrange] . $_POST[histos];

$dirs = explode("/", $topdir);                // split directory 
$sub  = explode(".", $dirs[sizeof($dirs)-1]); // last item of directory string
$subsystem = $sub[0];                         // eliminate .png

// on logbook.phenix.bnl.gov I need to change the directory listing where I find the plots
// into the correct URL
//
// http://logbook.phenix.bnl.gov/common/s6/backup/OnlMon/html/pedestaldata/run_0000327000_0000328000/327814/MUTRGMON_0_327814.png
// into 
// http://logbook.phenix.bnl.gov/OnlMon/pedestaldata/run_0000327000_0000328000/327814/MUTRGMON_0_327814.png

$cmd = "ls -r ".$topdir;

exec($cmd, $all_files);
foreach ( $dirs as $subdir){
  if( stristr($subdir, "data")) break;
}

//$runnumber = 123456;
//$url = "https://www.phenix.bnl.gov/WWW/run/10/online_monitoring/200GeV/mon.cgi?runnumber=".$runnumber."&runtype=".$subdir;
//$url = "http://logbook.phenix.bnl.gov/OnlMon/mon.cgi?runnumber=".$runnumber."&runtype=".$subdir;
//print "handling $topdir , $subdir, $subsystem, $url \n";

$old_runnumber = 0;

echo("<html>\n<head>\n</head>\n<body>\n");
echo("<center>\n");
echo("<h1> OnlMon Summary for $subsystem </h1>\n");
echo("<center> Selection was:  $topdir </center>\n");
echo("<table cellpadding=2 border=1>\n");

foreach ( $all_files as $file ){
  $dirs      = explode("/", $file);     // split filename 
  $runnumber = $dirs[sizeof($dirs)-2];  // second but last item of directory string
  $file_1008 = "http://logbook.phenix.bnl.gov/OnlMon/".$dirs[6]."/".$dirs[7]."/".$dirs[8]."/".$dirs[9]; // the URL for the file is different from disk location

  if( $runnumber != $old_runnumber){
//     $url = "https://www.phenix.bnl.gov/WWW/run/10/online_monitoring/200GeV/mon.cgi?runnumber=".$runnumber."&runtype=".$subdir;
//     $url = "http://logbook.phenix.bnl.gov/OnlMon/mon.cgi?runnumber=".$runnumber."&runtype=".$subdir;
     $url = "/OnlMon/mon.cgi?runnumber=".$runnumber."&runtype=".$subdir;
     echo("<tr>\n<th colspan=\"$ncolumns\" bgcolor=\"lightgrey\"> <center> <a href=\"$url\" target=\"_blank\"> Run : $runnumber </a> </center></th>\n</tr>\n<tr>\n");
     $at_column = 0;
     $old_runnumber = $runnumber;
  }
  echo("<td><a href=\"$file_1008\" target=\"_blank\"><img src=\"$file_1008\" width=\"$scale\" alt=\"$file_1008\"></a></td>");
    $at_column = $at_column + 1;
    if($at_column == $ncolumns){
        echo("</tr>\n<tr>\n");
        $at_column = 0;
    }

}

echo("</tr>\n");
echo("</table>\n");
echo("</html>\n");

?>


