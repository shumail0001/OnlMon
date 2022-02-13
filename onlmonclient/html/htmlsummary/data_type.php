<form name="datatypeselect" method="post" action="runrange_select.php" target="topright">
<center>
<p>
Select datatype from dropdown list <br>
 <select name="datatype">

<?php

//$run_directory = "/phenix/WWW/run/10/online_monitoring/200GeV/";
$run_directory = "/common/s6/backup/OnlMon/html/";

if( $dh = opendir($run_directory))
{
    while (($file = readdir($dh))!==false) 
    { 
//      echo("   " . $run_directory . " - " . $file . " <br> \n"); 
// Skip '.' and '..'
      if( $file == '.' || $file == '..') continue;
      if( is_dir($run_directory.$file) && stristr($file, "data") && !(stristr($file, "reject")) && !(stristr($file, "unknown")))
      {
        echo("<option value=\"" . $run_directory.$file . "\">" . $file . "</option>\n");
      }
    }
    closedir($dh);
}

?>
 
</select>
<br><input type="submit" name="Submit" value="Select">
</center>
</form> 


