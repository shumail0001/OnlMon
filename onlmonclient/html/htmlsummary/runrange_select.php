<table>
<tr>
<td>
<form name="subsystemselect" method="post" action="showplots.php" target="main">
<center>
<p>

Select subsystem from dropdown list <br>
 <select name="histos">

<?php


 $subsys = array(
                 "ACCMON_",
                 "BBCLVL1MON_",
                 "BbcMON_",
                 "DAQ_[0-9,c,l]",
                 "DCHMON_",
                 "EMCalMON_",
                 "ERTLVL1MON_",
                 "ERTMaskMon_",
                 "GL1MON_",
                 "LOCALPOLMON_0_",
                 "MCRMON_",
                 "MPCMON_",
                 "MUIDLVL1MON_",
                 "MUIDMONITOR_",
                 "MUTRGMON_",
                 "MUTRIGLVL1MON_",
                 "MUTRMON_[0-2]",
                 "PADMON_display",
                 "PBGLUMON_",
                 "PKTSIZEMON_",
                 "RICHMON_",
                 "RPCMON_",
                 "TOF",
                 "TOFWMON_",
                 "ZDCLVL1MON_",
                 "ZdcMON_[0,8]"
                );

 echo("   " . $run_directory. "<br>\n");
 foreach( $subsys as $sub){
   echo("<option value=\"" . $run_directory."/*/".$sub."*.png\"> " . $sub . " </option>\n");
 }
 
echo("</select>\n");
echo("</td>\n");

echo("<td>\n");
echo("<hr width=50>\n");
echo("</td>\n");

echo("<td>\n");

echo("<form name=\"runselect\" method=\"post\" action=\"showplots.php\" target=\"main\">\n");
echo("<center>\n");

echo("Select runrange from dropdown list <br>\n");
echo("<select name=\"runrange\">\n");

$run_directory = $_POST[datatype];

echo("   " . $run_directory. "<br>\n");

$file_list = array();

if( $dh = opendir($run_directory))
{
    while( false !== ($file = readdir($dh)))
    { 
        echo("   " . $file . "<br>\n");
        // Skip '.' and '..'
        if( $file == '.' || $file == '..') continue;
        if( is_dir($run_directory."/".$file) && stristr($file, "un_0"))
        {
          array_push($file_list, $file);
        }
    }
    closedir($dh);
}

rsort($file_list);

foreach ( $file_list as $file)
{ 
    echo("   " . $file . "<br>\n");
    echo("<option value=\"" . $run_directory."/".$file . "\">" . $file . "</option>\n");
}

?>
 
</select>
<br><input type="submit" name="Submit" value="Select">
</center>
</form> 
</td>
</tr>
</table>

