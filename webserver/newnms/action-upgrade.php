<!doctype html>
<html>
<head>
    <meta charset="UTF-8">
    <title>upgrade</title>
</head>
<body>

<?php
// 处理增加操作的页面 
if ($_FILES["file"]["error"] > 0)
{
    echo "错误：" . $_FILES["file"]["error"] . "<br>";
    echo "<meta http-equiv='Refresh' content='0;URL=upgrade.php'>";
}
else
{   $filename=$_FILES['file']['name'];
	$type=$_FILES['file']['type'];
	$tmp_name=$_FILES['file']['tmp_name'];
	$size=$_FILES['file']['size'];
	$error=$_FILES['file']['error'];
	$arr=explode(".", $_FILES["file"]["name"]);
	$hz=$arr[count($arr)-1];
	$randname=date("Y").date("m").date("d").date("H").date("i").date("s").rand(100, 999).$filename;

	if($error === 0){
		//把文件上传到./dir目录
		//该目录是否存在  || 不存在创建该目录	
		$up_root = './dir/';
		is_dir($up_root) || mkdir($up_root);
		//遍历上传的文件
		move_uploaded_file($tmp_name, "/var/".$randname);
		echo "上传文件名: " . $_FILES["file"]["name"] . "<br>";
		echo "文件类型: " . $_FILES["file"]["type"] . "<br>";
		echo "文件大小: " . ($_FILES["file"]["size"] / 1024) . " kB<br>";
		echo "文件临时存储的位置: " . $_FILES["file"]["tmp_name"];
		
	
    }
	  else{
      echo "文件异常，上传失败";
       }
    


    // 连接mysql
	$link = mysqli_connect("localhost","root","xlh123xlh","nmsdatabase") or die("提示：数据库连接失败！");
	// 选择数据库
	mysqli_select_db($link,"control");
	// 编码设置
	mysqli_set_charset($link,'utf8');

	// 获取增加的新闻

	$a=$_POST["s"];
	$size = count($a);
	$dir = "/var";
	$str = implode(",",$a);
	$str1 = $str.",".$dir. "/".$randname;
	$k = date("Y-m-d");
	$fh = mysqli_query($link,"select MAX(id) from control");
	$c_echo = mysqli_fetch_array($fh);
	$t = $c_echo[0];
	$b = $t + 1;
	// 插入数据
	mysqli_query($link,"insert into control(id,object,number,operation,time,state,content) values ('{$b}','4','{$len}','0','{$k}','0','{$str1}')") or die('添加数据出错：'.mysqli_error());  
	mysqli_close($link);
}

?>
<script type="text/javascript">
   window.setTimeout("window.location='upgrade.php'",15000);
</script>
</body>
</html>
