<?php
$command = "python3 tableclear.py 2>&1";
$output = shell_exec($command);
echo "<pre>$output</pre>";
header("Location: threshold_interface.html");
?>
