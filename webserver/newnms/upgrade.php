<!DOCTYPE html>                                                                                                                                                                  
 <html>
<head lang="en">  
     <meta charset="UTF-8">  
	 <title>添加用户</title>  
</head>
<style type="text/css">
	 form{
        margin: 20px;
     }
</style>
<body>
<form action="action-upgrade.php" method="post" enctype="multipart/form-data">
    <label>APserisls_ID：</label>             
       <?php

         // 2. 连接mysql
        $link = mysqli_connect("localhost","root","xlh123xlh","nmsdatabase") or die("提示：数据库连接失败！".mysqli_error());
         // 选择数据库
         mysqli_select_db($link,"AP");
         // 编码设置
         mysqli_set_charset($link,'utf8');
 
         // 3. 从DBNAME中查询到news数据库，返回数据库结果集,并按照addtime降序排列  
       $sql = 'select * from AP ';
         // 结果集
         $result = mysqli_query($link,$sql);

          
 
          
 
         ?>
 
          
 

 
               
 
              <?php while($row=mysqli_fetch_array($result)):?>
 
              <input name="s[]" type="checkbox" value="<?=$row['serials_ID']?>"><?=$row['serials_ID']?><br>

            <?php endwhile?>
        <label>文件名：</label>
        <input type="file" name="file" id="file"><br>  
        <input type="submit" value="提交"> 
    

</form>
 <?php

         // 2. 连接mysql
        $link = mysqli_connect("localhost","root","xlh123xlh","nmsdatabase") or die("提示：数据库连接失败！".mysqli_error());
         // 选择数据库
         mysqli_select_db($link,"control");
         // 编码设置
         mysqli_set_charset($link,'utf8');
 
         // 3. 从DBNAME中查询到news数据库，返回数据库结果集,并按照addtime降序排列  
         $fh = mysqli_query($link,"select MAX(id) from control");
         $c_echo = mysqli_fetch_array($fh);
         $t =$c_echo[0];
         $test = mysqli_query($link,"select * from control where id ='{$t}' ");
	 $tar = mysqli_fetch_array($test);
         $obj = $tar[1];
         echo $obj;
         $ope = $tar[3];
         echo $ope;
         $sta = $tar[5];
         echo $sta;
         $cont = $tar[6];
	 $d = explode(',', $cont);
         $tt = count($d) - 1;
         $myfile =$d[$tt];
         echo $myfile;

         if($obj == 4  &&  $ope == 0){
                if($sta == 2){
                    $openfile = fopen("{$myfile}", "r") or die ("Couldn't open the file");
                    $sp_data = fread($openfile,filesize($myfile));
                    echo "升级成功，可以操作其他，谢谢！" ;
                    
                }
                elseif($sta == 5){
		 echo "升级失败，可以操作其他，谢谢！" ;		
		}
                else{
                    echo "正在升级，请勿操作，稍等片刻，谢谢！" ;
                }

         }
         else{
            echo "暂时未有升级操作，可执行操作，操作后请勿连续操作，稍等片刻,谢谢！";
         }

 
          
 
         ?>


    <script type="text/javascript">
        window.setTimeout("window.location='log.php'",30000);
   </script>
</body>
</html>
