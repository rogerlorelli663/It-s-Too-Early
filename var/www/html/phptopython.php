<?php
$upthreshold = $_GET["threshold1"];
$lowthreshold = $_GET["threshold2"];
$command = "python3 project.py ".$upthreshold." ".$lowthreshold." 2>&1"; 
echo $command;
$output = shell_exec($command);
echo "<pre>$output</pre>";
header("Location: threshold_interface.html");
?>
