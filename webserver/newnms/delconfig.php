<?php
// 处理删除操作的页面 
require "dbconfig.php";
// 连接mysql
$link = mysqli_connect("localhost","root","xlh123xlh","nmsdatabase")  or die("提示：数据库连接失败！".mysqli_error());
// 选择数据库
mysqli_select_db($link,"config");
mysqli_select_db($link,"control");
// 编码设置
mysqli_set_charset($link,'utf8');

$template_ID = $_GET['template_ID'];
//删除指定数据  

$k = date("Y-m-d");
$fh = mysqli_query($link,"select MAX(id) from control");
$c_echo = mysqli_fetch_array($fh);
$t = $c_echo[0];
$b = $t + 1;
mysqli_query($link,"DELETE FROM config WHERE template_ID={$template_ID}") or die('删除数据出错：'.mysqli_error()); 
mysqli_query($link,"insert into control(id,object,number,operation,time,state,content) values ('{$b}','0','1','1','{$k}','0','{$template_ID}')") or die('添加数据出错：'.mysqli_error());  
// 删除完跳转到新闻页
header("Location:config.php");
?>  