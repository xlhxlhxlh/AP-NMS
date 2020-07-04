/*
auth:	Baojinzhuang
date:	2020-01-07
history:	
*/

#include<stdio.h>
#include<stdlib.h> 
#include<unistd.h>
#include<string.h>
#include<time.h>
#include<stdarg.h>
#include<ctype.h>


#include"define.h"

extern AP_Configuration_template apTemplate[max_template];
extern AP ap_list[max_ap];
extern int legalTemplate[max_template];
extern int legalAP[max_ap];
extern int legalAPServerConfigureResult[max_ap];//0，未配置，1，配置成功，2，配置失败，3，配置超时
extern APServer_timer legalAPServerTimer[max_ap_server];

extern template_operation_ack Template_Ack[max_template];
extern legalAP_operation_ack legalAP_Ack[max_ap];
extern AP_upgrade_ack legalAP_upgrade_Ack[max_ap];
extern AP_configuration_ack AP_Configure_Ack[max_ap];


extern APServer_socket apServer_socket_list[max_ap];
extern int sockSer;

extern int sockWeb;
extern struct sockaddr_in addrWebSer;  //创建一个记录地址信息的结构体 
extern socklen_t addrWeblen;

extern char sql_insert[1024];
extern MYSQL mysql;
extern MYSQL* conn;
extern MYSQL_RES resultt;
extern MYSQL_RES* result;
extern MYSQL_ROW row;
extern MYSQL_ROW row1;
extern MYSQL_RES r;
extern MYSQL_RES *result1;


void send_to_apServer(int apServerIndex, int command, AP_Configuration_template* tempTemplate, template_operation_req* templateOperation, legalAP_operation_req* APOperation, int size, char* apConfiguredList);
int print_log(unsigned int level, const char* filename, unsigned int line, char* format, ...);

void print_info(void);
void update_legalAP_add_o(AP* AP_list);
void update_legalAP_add(legalAP_operation_req* AP_req);
void update_legalAP_del(char* index);

void update_legalTemplate_del(int index);
void update_legalTemplate_add(template_operation_req* template_req);
void update_legalTemplate_add_o(AP_Configuration_template* ap_Template);

int Fill_ap_list();
int Fill_apTemplate();


int mysql_connect()//连接数据库
{
	int ret = -1;
	conn = mysql_init(&mysql);
	if (conn == NULL) 
	{
		ret = mysql_errno(&mysql);
		printf("mysql_init error: %s\n", mysql_error(&mysql));
		return ret;
	}
	mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "gbk");//编码设置
	conn = mysql_real_connect(conn, "127.0.0.1", "root", "xlh123xlh", "nmsdatabase", 0, NULL, 0);
	if (conn == NULL) 
	{
		ret = mysql_errno(&mysql);
		printf("mysql_real_connect error, err is: %s\n", mysql_error(&mysql));
		return ret;
	}
	printf("mysql_real_connect ok...\n");//链接数据库完成
	return ret;
}

int Fill_apTemplate()  //将模板数据写入到apTemplate数组中
{
	int i = 0;
	int ret = -1;
	ret = mysql_query(conn, "select * from config");
	if (ret != 0) {
		printf("mysql_select error: %s\n", mysql_error(&mysql));
	}
	result = mysql_store_result(&mysql);
	if (result == NULL)
	{
		printf("mysql_store_result error_97: %s\n", mysql_error(&mysql));
	}
	while ((row = mysql_fetch_row(result)))
	{
		(&apTemplate[i])->templateIndex = atoi(&row[0][0]);
		strcpy((&apTemplate[i])->ap_ssid.ssid, &row[1][0]);
		strcpy(((&apTemplate[i]))->ap_ssid.ssid_psw, &row[2][0]);
		strcpy((&apTemplate[i])->ap_login.login, &row[3][0]);
		strcpy((&apTemplate[i])->ap_login.login_psw, &row[4][0]);
		(&apTemplate[i])->NatOrBridge = atoi(&row[5][0]);
		(&apTemplate[i])->pool.start.s_addr = inet_addr(&row[6][0]);
		(&apTemplate[i])->pool.end.s_addr = inet_addr(&row[7][0]);
		(&apTemplate[i])->DHCPOrStatic = atoi(&row[8][0]);
		(&apTemplate[i])->AP_IP.s_addr = inet_addr(&row[9][0]);
		(&apTemplate[i])->ap_gateway.gateway.s_addr = inet_addr(&row[10][0]);
		(&apTemplate[i])->AP_Server_IP.s_addr = inet_addr(&row[11][0]);
		(&apTemplate[i])->ap_gateway.subnetmask.s_addr = inet_addr(&row[12][0]);
		printf("Fill_apTemplate AP_SSID:%s\n",&row[1][0]);
		i++;
	}
	mysql_free_result(result);
	return (i);
}

int Fill_ap_list()//将数据库的AP数据写入ap数组
{
	int i = 0;
	int ret = -1;
	ret = mysql_query(conn, "select * from AP");
	if (ret != 0) 
	{
		printf("mysql_select error: %s\n", mysql_error(&mysql));
	}
	result = mysql_store_result(&mysql);
	if (result == NULL)
	{
		printf("mysql_store_result error: %s\n", mysql_error(&mysql));
	}
	while ((row = mysql_fetch_row(result)))
	{
		strcpy((&ap_list[i])->AP_SN, &row[0][0]);
		(&ap_list[i])->configure.templateIndex = atoi(&row[1][0]);
		strcpy((&ap_list[i])->configure.ap_ssid.ssid, &row[2][0]);
		strcpy((&ap_list[i])->configure.ap_ssid.ssid_psw, &row[3][0]);
		strcpy((&ap_list[i])->configure.ap_login.login, &row[4][0]);
		strcpy((&ap_list[i])->configure.ap_login.login_psw, &row[5][0]);
		(&ap_list[i])->configure.NatOrBridge = atoi(&row[6][0]);
		(&ap_list[i])->configure.pool.start.s_addr = inet_addr(&row[7][0]);
		(&ap_list[i])->configure.pool.end.s_addr = inet_addr(&row[8][0]);
		(&ap_list[i])->configure.DHCPOrStatic = atoi(&row[9][0]);
		(&ap_list[i])->configure.AP_IP.s_addr = inet_addr(&row[10][0]);
		(&ap_list[i])->configure.ap_gateway.gateway.s_addr = inet_addr(&row[11][0]);
		(&ap_list[i])->configure.AP_Server_IP.s_addr = inet_addr(&row[12][0]);
		(&ap_list[i])->status.online_state = row[13][0];
		(&ap_list[i])->status.user_number = row[14][0];
		(&ap_list[i])->status.model = row[15][0];
		strcpy((&ap_list[i])->status.version, &row[16][0]);
		(&ap_list[i])->status.antenna_number = row[17][0];
		(&ap_list[i])->configure.ap_gateway.subnetmask.s_addr = inet_addr(&row[18][0]);
		printf("Fill_ap_list AP_Template_index:%d\n", row[1][0]);
		i++;
	}
	mysql_free_result(result);
	return (i);
}


int control_command_to_APserver()//读取Control表的命令，进行解析判断，并发送给对应的AP_Server
{
	template_operation_req template_req[max_template];
	legalAP_operation_req  legalAP_req[256];
	AP_Configuration_template ap_template[256];
	char username[50] = "root@192.168.0.96";
	char pwd[20] = "xlh";
	int size =-1;
	int i=0,k = 0,j=0,t=0;
	int obj=-1;
	int index=-1;
	int operation=-1;
	int ret=-1;
	int command=-1;
	char res[200] = { 0 };
	char input[15][30];
	//char a = 'a';
	char* pn = NULL;
	char* tn = NULL;
	char *gn = NULL;
	ret = mysql_query(conn, "select * from control where state = 0");
	if (ret != 0) {
		printf("mysql_select error: %s\n", mysql_error(&mysql));
		mysql_connect();
	}
	result = mysql_store_result(&mysql);
	if (result == NULL)
	{
		printf("Control表中没有命令_184\n");
	}
	//command_rows = mysql_num_rows(result);//取出的命令的行数
	//int field_num = mysql_field_count(&mysql); //列数
	// row[0]命令的序号  row[1] 对象 row[2] 数量 row[4] 时间  row[3] 操作 row[5] 执行状态  row[6] content
	//update grade set id = 1 where id =111
	else
	{
		while ((row = mysql_fetch_row(result)))
		{
			i = 0, k = 0, j = 0, index = -1;
			obj = atoi(&row[1][0]);
			size = atoi(&row[2][0]);
			operation = atoi(&row[3][0]);
			switch (obj)
			{
			case Template:
				command = APSERVER_WEB_TEMPLATE_MODIFY;
				switch (operation)
				{
					//template_ID	SSID_name	SSID_psw	Login_name	Login_psw	NatOrBridge	pool_start	pool_end	DhcpOrStatic	ip_address	gateway	APServer_IP
				case add:
					snprintf(sql_insert, 512, "select * from config where template_ID in (%s)", &row[6][0]);
					mysql_query(conn, sql_insert);
					result1 = mysql_store_result(&mysql);
					if (result1 == NULL) 
					{
						printf("mysql_store_result1 error: %s\n", mysql_error(&mysql));
						printf("Config表中没有命令_184\n");
					}
					else
					{
						printf("sql_insert is_216:%s\n", sql_insert);
						row1 = mysql_fetch_row(result1);
						while (size)
						{
							(&template_req[i])->command = TEMPLATE_ADD;
							(&template_req[i])->ap_template.templateIndex = atoi(&row1[0][0]);
							strcpy((&template_req[i])->ap_template.ap_ssid.ssid, &row1[1][0]);
							strcpy((&template_req[i])->ap_template.ap_ssid.ssid_psw, &row1[2][0]);
							strcpy((&template_req[i])->ap_template.ap_login.login, &row1[3][0]);
							strcpy((&template_req[i])->ap_template.ap_login.login_psw, &row1[4][0]);
							(&template_req[i])->ap_template.NatOrBridge = atoi(&row1[5][0]);
							(&template_req[i])->ap_template.pool.start.s_addr = inet_addr(&row1[6][0]);
							(&template_req[i])->ap_template.pool.end.s_addr = inet_addr(&row1[7][0]);
							(&template_req[i])->ap_template.DHCPOrStatic = atoi(&row1[8][0]);
							(&template_req[i])->ap_template.AP_IP.s_addr = inet_addr(&row1[9][0]);
							(&template_req[i])->ap_template.ap_gateway.gateway.s_addr = inet_addr(&row1[10][0]);
							(&template_req[i])->ap_template.AP_Server_IP.s_addr = inet_addr(&row1[11][0]);
							(&template_req[i])->ap_template.ap_gateway.subnetmask.s_addr = inet_addr(&row1[12][0]);
							for (index = 0; index < max_ap_server; index++)//找出AP所属的AP_Server的序列号
							{
								send_to_apServer(index, command, NULL, &template_req[i], NULL, 1, NULL);
							}
						    printf("AP_Server的IP为:%s\n", &row1[11][0]);
							update_legalTemplate_add(&template_req[i]);
							row1 = mysql_fetch_row(result1);
							size--; i++;
						}
					}
					mysql_free_result(result1);
					break;

				case del:
					(&template_req[i])->command = TEMPLATE_DEL;
					(&template_req[i])->ap_template.templateIndex = atoi(&row[6][0]);
					for (index = 0; index < max_ap_server; index++)
					{
						send_to_apServer(index, command, NULL, &template_req[i], NULL, 1, NULL);
					}
					snprintf(sql_insert, 512, "update control set state = 2 where id = %d", atoi(&row[0][0]));
					printf("del template sql_insert is:%s\n", sql_insert);
					mysql_query(conn, sql_insert);
					update_legalTemplate_del((&template_req[i])->ap_template.templateIndex);
					break;

				case modify:
					snprintf(sql_insert, 512, "select * from config where template_ID in (%s)", &row[6][0]);
					mysql_query(conn, sql_insert);
					result1 = mysql_store_result(&mysql);
					if (result1 == NULL) 
					{
						printf("mysql_store_result1 error_276: %s\n", mysql_error(&mysql));
						printf("sql_insert is:%s\n", sql_insert);
					}
					else 
					{
						printf("sql_insert is:%s\n", sql_insert);
						row1 = mysql_fetch_row(result1);
						while (size)
						{
							(&template_req[i])->command = TEMPLATE_MODIFY;
							(&template_req[i])->ap_template.templateIndex = atoi(&row1[0][0]);
							strcpy((&template_req[i])->ap_template.ap_ssid.ssid, &row1[1][0]);
							strcpy((&template_req[i])->ap_template.ap_ssid.ssid_psw, &row1[2][0]);
							strcpy((&template_req[i])->ap_template.ap_login.login, &row1[3][0]);
							strcpy((&template_req[i])->ap_template.ap_login.login_psw, &row1[4][0]);
							(&template_req[i])->ap_template.NatOrBridge = atoi(&row1[5][0]);
							(&template_req[i])->ap_template.pool.start.s_addr = inet_addr(&row1[6][0]);
							(&template_req[i])->ap_template.pool.end.s_addr = inet_addr(&row1[7][0]);
							(&template_req[i])->ap_template.DHCPOrStatic = atoi(&row1[8][0]);
							(&template_req[i])->ap_template.AP_IP.s_addr = inet_addr(&row1[9][0]);
							(&template_req[i])->ap_template.ap_gateway.gateway.s_addr = inet_addr(&row1[10][0]);
							(&template_req[i])->ap_template.AP_Server_IP.s_addr = inet_addr(&row1[11][0]);
							(&template_req[i])->ap_template.ap_gateway.subnetmask.s_addr = inet_addr(&row1[12][0]);
							for (index = 0; index < max_ap_server; index++)//找出AP的序列号
							{
								if (inet_addr(&row1[11][0]) == (&apServer_socket_list[index])->addrAP.sin_addr.s_addr)
									break;
							}
							if (index < 256)
							{
								send_to_apServer(index, command, NULL, &template_req[i], NULL, 1, NULL);
							}
							else
							{
								printf("error:未找到AP_Server的序列号,AP_Server的IP为：%s\n", &row1[11][0]);
							}
							update_legalTemplate_del((&template_req[i])->ap_template.templateIndex);
							update_legalTemplate_add(&template_req[i]);
							row1 = mysql_fetch_row(result1);
							size--; i++;
						}
					}
					mysql_free_result(result1);
					break;
				}
				break;

			case Ap:
				command = APSERVER_WEB_AP_MODIFY;
			/*	pn = strtok(&row[6][0], ",");
				while (pn)
				{
					strcpy(&input[k][0], pn);
					pn = strtok(NULL, ",");
					k++;
				}
				for (j = 0; j < k; j++)
				{
					strcat(res, "'");
					strcat(res, &input[j][0]);
					strcat(res, "'");
					strcat(res, ",");
				}
				res[strlen(res) - 1] = '\0';
				snprintf(sql_insert, 512, "select * from AP where serials_ID in (%s)", res);
				mysql_query(conn, sql_insert);
				result1 = mysql_store_result(&mysql);
				if (result1 == NULL)
				{
					printf("mysql_store_result error: %s\n", mysql_error(&mysql));
				}
				row1 = mysql_fetch_row(result1);*/

				switch (operation)
				{
				case add:
					//template_ID	SSID_name	SSID_psw	Login_name	Login_psw	NatOrBridge	pool_start	pool_end	DhcpOrStatic	ip_address	gateway	APServer_IP
					pn = strtok(&row[6][0], ",");
					while (pn)
					{
						strcpy(&input[k][0], pn);
						pn = strtok(NULL, ",");
						k++;
					}
					for (j = 0; j < k; j++)
					{
						strcat(res, "'");
						strcat(res, &input[j][0]);
						strcat(res, "'");
						strcat(res, ",");
					}
					res[strlen(res) - 1] = '\0';
					snprintf(sql_insert, 512, "select * from AP where serials_ID in (%s)", res);
					mysql_query(conn, sql_insert);
					result1 = mysql_store_result(&mysql);
					if (result1 == NULL)
					{
						printf("mysql_store_result error_362: %s\n", mysql_error(&mysql));
					}
					else
					{
						printf("sql_insert is_376:%s\n", sql_insert);
						row1 = mysql_fetch_row(result1);
						while (size)
						{
							(&legalAP_req[i])->command = AP_ADD;
							strcpy((&legalAP_req[i])->AP_SN, &row1[0][0]);
							(&legalAP_req[i])->ap_template.templateIndex = atoi(&row1[1][0]);
							strcpy((&legalAP_req[i])->ap_template.ap_ssid.ssid, &row1[2][0]);
							strcpy((&legalAP_req[i])->ap_template.ap_ssid.ssid_psw, &row1[3][0]);
							strcpy((&legalAP_req[i])->ap_template.ap_login.login, &row1[4][0]);
							strcpy((&legalAP_req[i])->ap_template.ap_login.login_psw, &row1[5][0]);
							(&legalAP_req[i])->ap_template.NatOrBridge = atoi(&row1[6][0]);
							(&legalAP_req[i])->ap_template.pool.start.s_addr = inet_addr(&row1[7][0]);
							(&legalAP_req[i])->ap_template.pool.end.s_addr = inet_addr(&row1[8][0]);
							(&legalAP_req[i])->ap_template.DHCPOrStatic = atoi(&row1[9][0]);
							(&legalAP_req[i])->ap_template.AP_IP.s_addr = inet_addr(&row1[10][0]);
							(&legalAP_req[i])->ap_template.ap_gateway.gateway.s_addr = inet_addr(&row1[11][0]);
							(&legalAP_req[i])->ap_template.AP_Server_IP.s_addr = inet_addr(&row1[12][0]);
							(&legalAP_req[i])->ap_template.ap_gateway.subnetmask.s_addr = inet_addr(&row1[18][0]);
							for (index = 0; index < max_ap_server; index++)//找出AP的序列号
							{
								send_to_apServer(index, command, NULL, NULL, &legalAP_req[i], 1, NULL);//调用Socket，将数据发送到ap_server
							}
							printf("AP_Server的IP为：%s\n", &row1[12][0]);
							update_legalAP_add(&legalAP_req[i]);
							row1 = mysql_fetch_row(result1);
							size--; i++;
						}
					}
					mysql_free_result(result1);
					break;
					//send_to_apServer(int apServerIndex, int command, AP_Configuration_template * tempTemplate, template_operation_req * templateOperation, legalAP_operation_req * APOperation, int size, char** apConfiguredList)
				case del:
					
					(&legalAP_req[i])->command = AP_DEL;
					strcpy((&legalAP_req[i])->AP_SN, &row[6][0]);
					for (index = 0; index < max_ap_server; index++)//找出AP的序列号
					{
						send_to_apServer(index, command, NULL, NULL, &legalAP_req[i], 1, NULL);
					}
					printf("del ap[%s] succeed!row[0][0]=%s\n",legalAP_req[i].AP_SN,&row[0][0]);
					printf("1234\n");
					snprintf(sql_insert, 512, "	update control set state = 2 where id = %d", atoi(&row[0][0]));
					printf("sql_insert is:%s\n", sql_insert);
					printf("12345\n");
					mysql_query(conn, sql_insert);
					update_legalAP_del(legalAP_req[i].AP_SN);
					printf("123456\n");
					break;

				case modify:
					pn = strtok(&row[6][0], ",");
					while (pn)
					{
						strcpy(&input[k][0], pn);
						pn = strtok(NULL, ",");
						k++;
					}
					for (j = 0; j < k; j++)
					{
						strcat(res, "'");
						strcat(res, &input[j][0]);
						strcat(res, "'");
						strcat(res, ",");
					}
					res[strlen(res) - 1] = '\0';
					snprintf(sql_insert, 512, "select * from AP where serials_ID in (%s)", res);
					mysql_query(conn, sql_insert);
					result1 = mysql_store_result(&mysql);
					if (result1 == NULL)
					{
						printf("mysql_store_result error_453: %s\n", mysql_error(&mysql));
						printf("sql_insert is:%s\n", sql_insert);
					}
					else
					{
						row1 = mysql_fetch_row(result1);
						while (size)
						{
							(&legalAP_req[i])->command = AP_MODIFY;
							strcpy((&legalAP_req[i])->AP_SN, &row1[0][0]);
							(&legalAP_req[i])->ap_template.templateIndex = atoi(&row1[1][0]);
							strcpy((&legalAP_req[i])->ap_template.ap_ssid.ssid, &row1[2][0]);
							strcpy((&legalAP_req[i])->ap_template.ap_ssid.ssid_psw, &row1[3][0]);
							strcpy((&legalAP_req[i])->ap_template.ap_login.login, &row1[4][0]);
							strcpy((&legalAP_req[i])->ap_template.ap_login.login_psw, &row1[5][0]);
							(&legalAP_req[i])->ap_template.NatOrBridge = atoi(&row1[6][0]);
							(&legalAP_req[i])->ap_template.pool.start.s_addr = inet_addr(&row1[7][0]);
							(&legalAP_req[i])->ap_template.pool.end.s_addr = inet_addr(&row1[8][0]);
							(&legalAP_req[i])->ap_template.DHCPOrStatic = atoi(&row1[9][0]);
							(&legalAP_req[i])->ap_template.AP_IP.s_addr = inet_addr(&row1[10][0]);
							(&legalAP_req[i])->ap_template.ap_gateway.gateway.s_addr = inet_addr(&row1[11][0]);
							(&legalAP_req[i])->ap_template.AP_Server_IP.s_addr = inet_addr(&row1[12][0]);
							(&legalAP_req[i])->ap_template.ap_gateway.subnetmask.s_addr = inet_addr(&row1[18][0]);
							for (index = 0; index < max_ap_server; index++)//找出AP的序列号
							{
								if (inet_addr(&row1[12][0]) == (&apServer_socket_list[index])->addrAP.sin_addr.s_addr)
									break;
							}
							if (index < 256)
							{
								send_to_apServer(index, command, NULL, NULL, &legalAP_req[i], 1, NULL);
							}
							else
							{
								printf("error:未找到AP_Server的序列号,AP_Server的IP为_486：%s\n", &row1[12][0]);
							}
							update_legalAP_del((&legalAP_req[i])->AP_SN);
							update_legalAP_add(&legalAP_req[i]);
							row1 = mysql_fetch_row(result1);
							size--; i++;
						}
					}
					mysql_free_result(result1);
					break;
				}
				memset(res, 0, sizeof(res));
				break;

			case configuring:
				command = APSERVER_WEB_AP_CONFIGURE;
				mysql_query(conn, "select * from temporary");
				result1 = mysql_store_result(&mysql);
				if (result1 == NULL)
				{
					printf("mysql_store_result error_505: %s\n", mysql_error(&mysql));
					printf("sql_insert is:%s\n", sql_insert);
				}
				else
				{
					row1 = mysql_fetch_row(result1);
					//template_ID	SSID_name	SSID_psw	Login_name	Login_psw	NatOrBridge	pool_start	pool_end	DhcpOrStatic	ip_address	gateway	APServer_IP
					strcpy((&ap_template[i])->ap_ssid.ssid, &row1[0][0]);
					strcpy((&ap_template[i])->ap_ssid.ssid_psw, &row1[1][0]);
					strcpy((&ap_template[i])->ap_login.login, &row1[2][0]);
					strcpy((&ap_template[i])->ap_login.login_psw, &row1[3][0]);
					(&ap_template[i])->NatOrBridge = atoi(&row1[4][0]);
					(&ap_template[i])->pool.start.s_addr = inet_addr(&row1[5][0]);
					(&ap_template[i])->pool.end.s_addr = inet_addr(&row1[6][0]);
					(&ap_template[i])->DHCPOrStatic = atoi(&row1[7][0]);
					(&ap_template[i])->AP_IP.s_addr = inet_addr(&row1[8][0]);
					(&ap_template[i])->ap_gateway.gateway.s_addr = inet_addr(&row1[9][0]);
					(&ap_template[i])->AP_Server_IP.s_addr = inet_addr(&row1[10][0]);
					(&ap_template[i])->ap_gateway.subnetmask.s_addr = inet_addr(&row1[11][0]);
					pn = strtok(&row[6][0], ",");
					while (pn)
					{
						strcpy(&input[k][0], pn);
						pn = strtok(NULL, ",");
						k++;
					}
					for (j = 0; j < k; j++)//找到AP_server IP 所属AP_Server的序列号
					{
						//strcpy((&legalAP_req[255])->AP_SN, &input[j][0]);
						//update_legalAP_add(&legalAP_req[255]);//添加到AP表格
						snprintf(sql_insert, 512, "select Apserver_IP from AP where serials_ID = '%s'", &input[j][0]);
						printf("sql_insert is:%s\n", sql_insert);
						mysql_query(conn, sql_insert);
						result1 = mysql_store_result(&mysql);
						if (result1 == NULL)
						{
							printf("mysql_store_result error_540: %s\n", mysql_error(&mysql));
						}
						else
						{
							row1 = mysql_fetch_row(result1);
							for (index = 0; index < max_ap_server; index++)//找出AP所属AP_Server的序列号
							{
								if (inet_addr(&row1[0][0]) == (&apServer_socket_list[index])->addrAP.sin_addr.s_addr)
									break;
							}
							if (index < 256)
							{
								send_to_apServer(index, command, &ap_template[i], NULL, NULL, 1, &input[i][0]);
								printf("Success! configure add SSID  apserver_index:%s,%d\n", (&ap_template[i])->ap_ssid.ssid, index);
							}
							else
							{
								printf("error:未找到AP_Server的序列号,AP_Server的IP为：%s\n", (&ap_template[i])->ap_ssid.ssid);
							}
						}
					}
				}
				memset(input, 0, sizeof(input));
				mysql_free_result(result1);
				break;
			case update_status:
				
				switch (operation)
				{
					case add:
					//template_ID	SSID_name	SSID_psw	Login_name	Login_psw	NatOrBridge	pool_start	pool_end	DhcpOrStatic	ip_address	gateway	APServer_IP
					pn = strtok(&row[6][0], ",");
					command = APSERVER_WEB_SoftWare_Upgrade;
					while (pn)
					{
						strcpy(&input[k][0], pn);
						pn = strtok(NULL, ",");
						k++;
					}
					t = k - 1;
					strcpy(tn, &input[t][0]);
					t = t - 1;
					strcpy(gn, &input[t][0]);
					for (j = 0; j < t; j++)
					{
						strcat(res, "'");
						strcat(res, &input[j][0]);
						strcat(res, "'");
						strcat(res, ",");
					}
					res[strlen(res) - 1] = '\0';
					snprintf(sql_insert, 512, "select * from AP where serials_ID in (%s)", res);
					mysql_query(conn, sql_insert);
					result1 = mysql_store_result(&mysql);
					if (result1 == NULL)
					{
						printf("mysql_store_result error_362: %s\n", mysql_error(&mysql));
					}
					else
					{
						printf("sql_insert is_376:%s\n", sql_insert);
						row1 = mysql_fetch_row(result1);
						while (size)
						{
							(&legalAP_req[i])->command = APSERVER_WEB_SoftWare_Upgrade;
							strcpy((&legalAP_req[i])->AP_SN, &row1[0][0]);
							(&legalAP_req[i])->ap_template.templateIndex = atoi(&row1[1][0]);
							strcpy((&legalAP_req[i])->ap_template.ap_ssid.ssid, &row1[2][0]);
							strcpy((&legalAP_req[i])->ap_template.ap_ssid.ssid_psw, &row1[3][0]);
							strcpy((&legalAP_req[i])->ap_template.ap_login.login, &row1[4][0]);
							strcpy((&legalAP_req[i])->ap_template.ap_login.login_psw, &row1[5][0]);
							(&legalAP_req[i])->ap_template.NatOrBridge = atoi(&row1[6][0]);
							(&legalAP_req[i])->ap_template.pool.start.s_addr = inet_addr(&row1[7][0]);
							(&legalAP_req[i])->ap_template.pool.end.s_addr = inet_addr(&row1[8][0]);
							(&legalAP_req[i])->ap_template.DHCPOrStatic = atoi(&row1[9][0]);
							(&legalAP_req[i])->ap_template.AP_IP.s_addr = inet_addr(&row1[10][0]);
							(&legalAP_req[i])->ap_template.ap_gateway.gateway.s_addr = inet_addr(&row1[11][0]);
							(&legalAP_req[i])->ap_template.AP_Server_IP.s_addr = inet_addr(&row1[12][0]);
							(&legalAP_req[i])->ap_template.ap_gateway.subnetmask.s_addr = inet_addr(&row1[18][0]);
							strcpy((&legalAP_req[i])->ap_template.Scp_Username, username);
							strcpy((&legalAP_req[i])->ap_template.Scp_PassWord, pwd);
							strcpy((&legalAP_req[i])->ap_template.Remote_FilePath, tn);
							strcpy((&legalAP_req[i])->ap_template.version, gn);
							//添加三个尾路径
							for (index = 0; index < max_ap_server; index++)//找出AP的序列号
							{
								send_to_apServer(index, command, NULL, NULL, &legalAP_req[i], 1, NULL);//调用Socket，将数据发送到ap_server
							}
							printf("AP_Server的IP为：%s\n", &row1[12][0]);
							update_legalAP_add(&legalAP_req[i]);
							row1 = mysql_fetch_row(result1);
							size--; i++;
						}
					}
					mysql_free_result(result1);
					break;
					//send_to_apServer(int apServerIndex, int command, AP_Configuration_template * tempTemplate, template_operation_req * templateOperation, legalAP_operation_req * APOperation, int size, char** apConfiguredList)
				case del:
					pn = strtok(&row[6][0], ",");
					command = APSERVER_WEB_Request_LogInfo;
					while (pn)
					{
						strcpy(&input[k][0], pn);
						pn = strtok(NULL, ",");
						k++;
					}

					for (j = 0; j < k; j++)
					{
						strcat(res, "'");
						strcat(res, &input[j][0]);
						strcat(res, "'");
						strcat(res, ",");
					}
					res[strlen(res) - 1] = '\0';
					snprintf(sql_insert, 512, "select * from AP where serials_ID in (%s)", res);
					mysql_query(conn, sql_insert);
					result1 = mysql_store_result(&mysql);
					if (result1 == NULL)
					{
						printf("mysql_store_result error_362: %s\n", mysql_error(&mysql));
					}
					else
					{
						printf("sql_insert is_376:%s\n", sql_insert);
						row1 = mysql_fetch_row(result1);
						while (size)
						{
							(&legalAP_req[i])->command = APSERVER_WEB_Request_LogInfo;
							strcpy((&legalAP_req[i])->AP_SN, &row1[0][0]);
							(&legalAP_req[i])->ap_template.templateIndex = atoi(&row1[1][0]);
							strcpy((&legalAP_req[i])->ap_template.ap_ssid.ssid, &row1[2][0]);
							strcpy((&legalAP_req[i])->ap_template.ap_ssid.ssid_psw, &row1[3][0]);
							strcpy((&legalAP_req[i])->ap_template.ap_login.login, &row1[4][0]);
							strcpy((&legalAP_req[i])->ap_template.ap_login.login_psw, &row1[5][0]);
							(&legalAP_req[i])->ap_template.NatOrBridge = atoi(&row1[6][0]);
							(&legalAP_req[i])->ap_template.pool.start.s_addr = inet_addr(&row1[7][0]);
							(&legalAP_req[i])->ap_template.pool.end.s_addr = inet_addr(&row1[8][0]);
							(&legalAP_req[i])->ap_template.DHCPOrStatic = atoi(&row1[9][0]);
							(&legalAP_req[i])->ap_template.AP_IP.s_addr = inet_addr(&row1[10][0]);
							(&legalAP_req[i])->ap_template.ap_gateway.gateway.s_addr = inet_addr(&row1[11][0]);
							(&legalAP_req[i])->ap_template.AP_Server_IP.s_addr = inet_addr(&row1[12][0]);
							(&legalAP_req[i])->ap_template.ap_gateway.subnetmask.s_addr = inet_addr(&row1[18][0]);

							//添加三个尾路径
							for (index = 0; index < max_ap_server; index++)//找出AP的序列号
							{
								send_to_apServer(index, command, NULL, NULL, &legalAP_req[i], 1, NULL);//调用Socket，将数据发送到ap_server
							}
							printf("AP_Server的IP为：%s\n", &row1[12][0]);
							update_legalAP_add(&legalAP_req[i]);
							row1 = mysql_fetch_row(result1);
							size--; i++;
						}
					}
				}
				    memset(input, 0, sizeof(input));
					mysql_free_result(result1);
					break;
			default:
				printf("undefined command AP Server to AP\n");
			}
		}//加一个大的case代表升级未修改完。
	}
	ret = mysql_query(conn, "update control set state = 1 where state = 0");
	if (ret != 0) 
	{
		printf("control_update error: %s\n", mysql_error(&mysql));
	}
	mysql_free_result(result);//释放内存

	return ret;
}


//调用Socket，将数据发送到ap_server
void send_to_apServer(int apServerIndex, int command, AP_Configuration_template* tempTemplate, template_operation_req* templateOperation, legalAP_operation_req* APOperation, int size, char* apConfiguredList)
{
	int i = 0;
	char sendbuf[sizeof(AP) + 100];    //申请一个发送数据缓存区   模板数据结构            配置模板数据结构
	char* p = sendbuf;

	*p = command;
	p++;
	*p = size;
	p++;

	switch (command)
	{
	case APSERVER_WEB_TEMPLATE_REPORT:     //10
		size=0;
		for(i=0;i<max_template;i++)
		{
		if(legalTemplate[i])
			{
			size++;
			memcpy(p,(char*)&apTemplate[i],sizeof(AP_Configuration_template));
			p+=sizeof(AP_Configuration_template);
			}
		}
		sendbuf[1]=size;
		sendto(sockSer, sendbuf, size * sizeof(AP_Configuration_template) + 2, 0, (struct sockaddr*) & apServer_socket_list[apServerIndex].addrAP, apServer_socket_list[apServerIndex].addrlen);
		break;
	case APSERVER_WEB_AP_REPORT:
		size=0;
		for(i=0;i<max_ap;i++)
		{
		if(legalAP[i])
			{
			size++;
			memcpy(p,(char*)&ap_list[i],sizeof(AP));
			p+=sizeof(AP);
			}
		}
		sendbuf[1]=size;
		sendto(sockSer, sendbuf, size * sizeof(AP) + 2, 0, (struct sockaddr*) & apServer_socket_list[apServerIndex].addrAP, apServer_socket_list[apServerIndex].addrlen);
		break;
	case APSERVER_WEB_TEMPLATE_MODIFY:
		for (i = 0; i < size; i++)
			memcpy(p + sizeof(template_operation_req) * i, templateOperation + i, sizeof(template_operation_req));
		sendto(sockSer, sendbuf, size * sizeof(template_operation_req) + 2, 0, (struct sockaddr*) & apServer_socket_list[apServerIndex].addrAP, apServer_socket_list[apServerIndex].addrlen);
		break;
	case APSERVER_WEB_AP_MODIFY:
		for (i = 0; i < size; i++)
			memcpy(p + sizeof(legalAP_operation_req) * i, APOperation + i, sizeof(legalAP_operation_req));
		sendto(sockSer, sendbuf, size * sizeof(legalAP_operation_req) + 2, 0, (struct sockaddr*) & apServer_socket_list[apServerIndex].addrAP, apServer_socket_list[apServerIndex].addrlen);
		break;
	case APSERVER_WEB_AP_CONFIGURE:
		memcpy(p, tempTemplate, sizeof(AP_Configuration_template));
		p = p + sizeof(AP_Configuration_template);
		memcpy(p, apConfiguredList, APSN_length);
		sendto(sockSer, sendbuf, sizeof(AP_Configuration_template) + 2 + size * APSN_length, 0, (struct sockaddr*) & apServer_socket_list[apServerIndex].addrAP, apServer_socket_list[apServerIndex].addrlen);
		break;
	case APSERVER_WEB_HEART_BEAT:
		sendto(sockSer, sendbuf, 2, 0, (struct sockaddr*) & apServer_socket_list[apServerIndex].addrAP, apServer_socket_list[apServerIndex].addrlen);
		break;
	case APSERVER_WEB_SoftWare_Upgrade:
		for (i = 0; i < size; i++)
			memcpy(p + sizeof(legalAP_operation_req) * i, APOperation + i, sizeof(legalAP_operation_req));
		sendbuf[1]=size;
		sendto(sockSer, sendbuf, size * sizeof(legalAP_operation_req) + 2, 0, (struct sockaddr*) & apServer_socket_list[apServerIndex].addrAP, apServer_socket_list[apServerIndex].addrlen);
		break;
	case APSERVER_WEB_Request_LogInfo:
		for (i = 0; i < size; i++)
			memcpy(p + sizeof(legalAP_operation_req) * i, APOperation + i, sizeof(legalAP_operation_req));
		sendbuf[1]=size;
		sendto(sockSer, sendbuf, size * sizeof(legalAP_operation_req) + 2, 0, (struct sockaddr*) & apServer_socket_list[apServerIndex].addrAP, apServer_socket_list[apServerIndex].addrlen);
		break;
	default:
		printf("undefined command AP Server to AP");
	}


}//这也要修改加case

int LegalAPServer_find(struct      sockaddr_in* apserver_socket_addr)//查找ap_sever的序列号
{
    int i=0;
	for(i=0;i<max_ap_server;i++)
		{
			if(apServer_socket_list[i].addrAP.sin_addr.s_addr==apserver_socket_addr->sin_addr.s_addr)
				return i;
		}
	return -1;
}

int assign_apIndex()//初始化ap
{
    int i=0;
	for(i=0;i<max_ap;i++)
		{
			if(legalAP[i]==0)
				return i;
		}
	return -1;

}

void ap_modify_report_assign_value(legalAP_operation_ack* destination, legalAP_operation_ack* source)//ack的赋值
{
	destination->ack = source->ack;
	strcpy(destination->AP_SN, source->AP_SN);
}
void ap_upgrade_report_assign_value(AP_upgrade_ack* destination, AP_upgrade_ack* source)//ack的赋值
{
	destination->ack = source->ack;
	strcpy(destination->AP_SN, source->AP_SN);
	strcpy(destination->Scp_Username, source->Scp_Username);
	strcpy(destination->Scp_PassWord, source->Scp_PassWord);
	strcpy(destination->Remote_FilePath, source->Remote_FilePath);	
}

void ap_configure_report_assign_value(AP_configuration_ack* destination,AP_configuration_ack* source)//配置赋值 把source的东西付给destination
{
	destination->ack = source->ack;
	strcpy(destination->AP_SN, source->AP_SN);
}

void template_modify_report_assign_value(template_operation_ack* destination,template_operation_ack* source)
{
	destination->ack = source->ack;
	destination->templateIndex = source->templateIndex;
}

void ap_configure_assign_value(AP* destination,AP* source)
{
	strcpy(destination->AP_SN, source->AP_SN);
	destination->configure.ap_gateway.gateway=source->configure.ap_gateway.gateway;
    destination->configure.ap_gateway.subnetmask=source->configure.ap_gateway.subnetmask;
	strcpy(destination->configure.ap_login.login,source->configure.ap_login.login);
	strcpy(destination->configure.ap_login.login_psw,source->configure.ap_login.login_psw);
	strcpy(destination->configure.ap_ssid.ssid,source->configure.ap_ssid.ssid);
	strcpy(destination->configure.ap_ssid.ssid_psw,source->configure.ap_ssid.ssid_psw);
	destination->configure.NatOrBridge = source->configure.NatOrBridge;
	destination->configure.pool.start = source->configure.pool.start;
	destination->configure.pool.end = source->configure.pool.end;
	destination->configure.templateIndex = source->configure.templateIndex;
	destination->configure.DHCPOrStatic = source->configure.DHCPOrStatic;
	destination->configure.AP_IP = source->configure.AP_IP;
	destination->configure.AP_Server_IP = source->configure.AP_Server_IP;
	destination->configure.options = source->configure.options;
	strcpy(destination->configure.Scp_Username, source->configure.Scp_Username);
	strcpy(destination->configure.Scp_PassWord, source->configure.Scp_PassWord);
    strcpy(destination->configure.Remote_FilePath, source->configure.Remote_FilePath);	
}


void template_assign_value(AP_Configuration_template* destination,AP_Configuration_template* source)
{
	destination->ap_gateway.gateway=source->ap_gateway.gateway;
    destination->ap_gateway.subnetmask=source->ap_gateway.subnetmask;
	strcpy(destination->ap_login.login,source->ap_login.login);
	strcpy(destination->ap_login.login_psw,source->ap_login.login_psw);
	strcpy(destination->ap_ssid.ssid,source->ap_ssid.ssid);
	strcpy(destination->ap_ssid.ssid_psw,source->ap_ssid.ssid_psw);
	destination->NatOrBridge = source->NatOrBridge;
	destination->pool.start = source->pool.start;
	destination->pool.end = source->pool.end;
	destination->templateIndex = source->templateIndex;
	destination->DHCPOrStatic = source->DHCPOrStatic;
	destination->AP_IP = source->AP_IP;
	destination->AP_Server_IP = source->AP_Server_IP;
	destination->options = source->options;
	strcpy(destination->Scp_Username, source->Scp_Username);
	strcpy(destination->Scp_PassWord, source->Scp_PassWord);
    stpcpy(destination->Remote_FilePath, source->Remote_FilePath);	
}//修改完成


void ap_status_assign_value(AP* destination,AP* source)//status 的赋值
{
	
	strcpy(destination->AP_SN, source->AP_SN);
	destination->status.antenna_number = source->status.antenna_number;
	destination->status.model = source->status.model;
	destination->status.user_number = source->status.user_number;
	destination->status.online_state = source->status.online_state;
	strcpy(destination->status.version,source->status.version);
	destination->status.options = source->status.options;
}


void decode_template_report(char* buf, AP_Configuration_template* temp)//解析apserver发送过来的模板
{
	int i=0,size=buf[1];
	char* p=buf;
	p++;p++;
	AP_Configuration_template* pp=(AP_Configuration_template*)p;
	for(i=0;i<size;i++)
		{
		template_assign_value(temp+i,pp+i);
		}
}

void decode_ap_report(char* buf, AP* temp)//解析apserver发送过来的ap
{
	
	int i=0,size=buf[1];
	char* p=buf;
	p++;p++;
	AP* pp=(AP*)p;
	for(i=0;i<size;i++)
	{
	  ap_status_assign_value(temp + i, pp + i);
	  ap_configure_assign_value(temp + i, pp + i);
	}
}

void decode_template_modify_report(char* buf)//解析apserver发送过来的更改模板确认信息
{

	int i=0,size=buf[1];
	char* p=buf;
	p++;p++;
	template_operation_ack* pp=(template_operation_ack*)p;
	for(i=0;i<size;i++)
		{
		template_modify_report_assign_value(&Template_Ack[i], pp + i);
		}
}

void decode_ap_status_reporter(char* buf,AP* temp)//解析apserver发送过来的状态信息
{

	int i = 0, size = buf[1];
	char* p = buf;
	p++; p++;
	AP* pp = (AP*)p;
	for (i = 0; i < size; i++)
	{
		ap_status_assign_value(temp + i, pp + i);
	}
}

void decode_ap_modify_report(char* buf)//解析apserver发送过来的更改ap确认信息 修改过
{
	int i=0,size=buf[1];
	char* p=buf;
	p++;p++;
	legalAP_operation_ack* pp=(legalAP_operation_ack*)p;
	for(i=0;i<size;i++)
		{
		ap_modify_report_assign_value(&legalAP_Ack[i],pp+i);
		}
}
void decode_ap_upgrade_report(char* buf)//解析apserver发送过来的更改ap确认信息 修改过
{
	int i=0,size=buf[1];
	char* p=buf;
	p++;p++;
	AP_upgrade_ack* pp=(AP_upgrade_ack*)p;
	for(i=0;i<size;i++)
		{
		ap_upgrade_report_assign_value(&legalAP_upgrade_Ack[i],pp+i);
		}
}

void decode_ap_configure_report(char* buf)//解析apserver发送过来的配置信息
{
	int i=0,size=buf[1];
	char* p=buf;
	p++;p++;
	AP_configuration_ack* pp=(AP_configuration_ack*)p;
	for(i=0;i<size;i++)
		{
		ap_configure_report_assign_value(&AP_Configure_Ack[i],pp+i);
		}
}//也要添加一个类似的函数


void  dispatch_APServerMsg(int apServer_index, char* buf)//处理apserver 发送过来的信息，将结果写入数据库
{
	//AP* pp=(AP*)(buf+1);
	char s[100] = "\0";
	char str[10] = ",";
	char temp[500] ;
	char temp1[5000];
	char *p1,*p2,*p3,*p4,*p5,*p6;
	char ip1[25] = "\0";
	char ip2[25] = "\0";
	char ip3[25] = "\0";
	char ip4[25] = "\0";
	char ip5[25] = "\0";
	char ip6[25] = "\0";
	int ret= -1;
	int i = 0;
	int j = 0;
	int k = 0;
	int status ;
	//int template_a = 0,template_b = 0;
	//int ap_a = 0,ap_b = 0;
	template_operation_req template_req[max_template];
	legalAP_operation_req  legalAP_req[256];

	AP list[256];
	AP_Configuration_template apTemplate[max_template];

	switch (buf[0])
	{
	case APSERVER_WEB_TEMPLATE_REPORT:
		legalAPServerTimer[apServer_index].apServer_timer_flag = WEB_APSERVER_CONNECTING;
		legalAPServerTimer[apServer_index].apServer_on_timer = web_apServer_configure_time;
		decode_template_report(buf,&apTemplate[0]);//解析apserver发送过来的模板
		/*
		char* p=buf;
		char ssid[ssid_length];
		char ssid_psw[ssid_psw_length];
		struct in_addr* ppp;
		p++;p++;
		//AP_Configuration_template* pp=(AP_Configuration_template*)p;
		memcpy(ssid,p,ssid_length);
		p+=ssid_length;
		printf("ssid=%s\n",ssid);
		memcpy(ssid_psw,&buf[2+ssid_length],ssid_psw_length);
		printf("ssidpsw=%s\n",ssid_psw);
		p+=ssid_psw_length;
		ppp=(struct in_addr*)p;
		printf("ip=%s\n",inet_ntoa(*ppp));
		printf("buf[0]=0x%x,buf[1]=0x%x,buf[2]=0x%x,buf[3]=0x%x,buf[4]=0x%x,buf[5]=0x%x\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);
		printf("buf[6]=0x%x,buf[7]=0x%x,buf[8]=0x%x,buf[9]=0x%x,buf[10]=0x%x,buf[11]=0x%x\n",buf[6],buf[7],buf[8],buf[9],buf[10],buf[11]);
		printf("buf[12]=0x%x,buf[13]=0x%x,buf[14]=0x%x,buf[15]=0x%x,buf[16]=0x%x,buf[17]=0x%x\n",buf[12],buf[13],buf[14],buf[15],buf[16],buf[17]);
		printf("buf[18]=0x%x,buf[19]=0x%x,buf[20]=0x%x,buf[21]=0x%x,buf[22]=0x%x,buf[23]=0x%x\n",buf[18],buf[19],buf[20],buf[21],buf[22],buf[23]);*/
		log_info("Info: APSERVER_WEB_TEMPLATE_REPORT,size=%d,\n",buf[1]);
		//write templdate to database
		for (i = 0; i < buf[1]; i++)
		{
			/*apTemplate[i].ap_ssid.ssid  apTemplate[i].ap_ssid.ssid_psw  apTemplate[i].ap_login.login  apTemplate[i].ap_login.login_psw  apTemplate[i].options
			apTemplate[i].DHCPOrStatic  apTemplate[i].NatOrBridge    apTemplate[i].pool.start  apTemplate[i].pool.end  apTemplate[i].ap_gateway.gateway变量*/
			strcpy(ip1, inet_ntoa(apTemplate[i].pool.start));
			strcpy(ip2, inet_ntoa(apTemplate[i].pool.end));
			strcpy(ip3, inet_ntoa(apTemplate[i].AP_IP));
			strcpy(ip4, inet_ntoa(apTemplate[i].ap_gateway.gateway));
			strcpy(ip5, inet_ntoa(apTemplate[i].AP_Server_IP));
			strcpy(ip6, inet_ntoa(apTemplate[i].ap_gateway.subnetmask));
			snprintf(sql_insert, 512, "INSERT INTO config (template_ID, SSID_name, SSID_psw, login_name, login_psw,\
			NATOrBride, pool_start, pool_end, dhcporstatic, ip_adress, gateway,APServer_IP,netmask) Values (%d,'%s','%s','%s','%s',%d,'%s','%s',%d,'%s','%s','%s','%s')", \
				apTemplate[i].templateIndex,apTemplate[i].ap_ssid.ssid, apTemplate[i].ap_ssid.ssid_psw, apTemplate[i].ap_login.login, apTemplate[i].ap_login.login_psw, \
				apTemplate[i].NatOrBridge, ip1, ip2, apTemplate[i].DHCPOrStatic,ip3, ip4, ip5,ip6);
			
			printf("sql_insert is；%s\n", sql_insert);
			ret = mysql_query(conn, sql_insert);//将数据写入数据库中
			if (ret == 0)
			{
				printf("template reporter insert success\n");
				update_legalTemplate_add_o(&apTemplate[i]);
				//printf("mysql_query error is: %s\n", mysql_error(&mysql));
			}
			memset(ip1, 0, sizeof(ip1)); memset(ip2, 0, sizeof(ip2)); memset(ip3, 0, sizeof(ip3)); memset(ip4, 0, sizeof(ip4)); memset(ip5, 0, sizeof(ip5)); memset(ip6, 0, sizeof(ip6));
		
		}
		mysql_query(conn, "select * from config");//发送给AP_Server
		result = mysql_store_result(&mysql);
		if (result == NULL)
		{
			printf("mysql_store_result error: %s\n", mysql_error(&mysql));
		}
		else
		{
			while ((row = mysql_fetch_row(result)))
			{
				(&template_req[i])->command = TEMPLATE_ADD;
				(&template_req[i])->ap_template.templateIndex = atoi(&row[0][0]);
				strcpy((&template_req[i])->ap_template.ap_ssid.ssid, &row[1][0]);
				strcpy((&template_req[i])->ap_template.ap_ssid.ssid_psw, &row[2][0]);
				strcpy((&template_req[i])->ap_template.ap_login.login, &row[3][0]);
				strcpy((&template_req[i])->ap_template.ap_login.login_psw, &row[4][0]);
				(&template_req[i])->ap_template.NatOrBridge = atoi(&row[5][0]);
				(&template_req[i])->ap_template.pool.start.s_addr = inet_addr(&row[6][0]);
				(&template_req[i])->ap_template.pool.end.s_addr = inet_addr(&row[7][0]);
				(&template_req[i])->ap_template.DHCPOrStatic = atoi(&row[8][0]);
				(&template_req[i])->ap_template.AP_IP.s_addr = inet_addr(&row[9][0]);
				(&template_req[i])->ap_template.ap_gateway.gateway.s_addr = inet_addr(&row[10][0]);
				(&template_req[i])->ap_template.AP_Server_IP.s_addr = inet_addr(&row[11][0]);
				(&template_req[i])->ap_template.ap_gateway.subnetmask.s_addr = inet_addr(&row[12][0]);
				printf("templateID=%d's ssid=%s\n", template_req[i].ap_template.templateIndex, template_req[i].ap_template.ap_ssid.ssid);
				printf("templateID=%d's ssid_psw=%s\n", template_req[i].ap_template.templateIndex, template_req[i].ap_template.ap_ssid.ssid_psw);
				printf("templateID=%d's login=%s\n", template_req[i].ap_template.templateIndex, template_req[i].ap_template.ap_login.login);
				printf("templateID=%d's login_psw=%s\n", template_req[i].ap_template.templateIndex, template_req[i].ap_template.ap_login.login_psw);
				printf("templateID=%d's NatOrBridge=%d\n", template_req[i].ap_template.templateIndex, template_req[i].ap_template.NatOrBridge);
				printf("templateID=%d's pool.start=%s\n", template_req[i].ap_template.templateIndex, inet_ntoa(template_req[i].ap_template.pool.start));
				printf("templateID=%d's pool.end=%s\n", template_req[i].ap_template.templateIndex, inet_ntoa(template_req[i].ap_template.pool.end));
				printf("templateID=%d's DHCPOrStatic=%d\n", template_req[i].ap_template.templateIndex, template_req[i].ap_template.DHCPOrStatic);
				printf("templateID=%d's ipaddr=%s\n", template_req[i].ap_template.templateIndex, inet_ntoa(template_req[i].ap_template.AP_IP));
				printf("templateID=%d's netmask=%s\n", template_req[i].ap_template.templateIndex, inet_ntoa(template_req[i].ap_template.ap_gateway.subnetmask));
				printf("templateID=%d's gateway=%s\n", template_req[i].ap_template.templateIndex, inet_ntoa(template_req[i].ap_template.ap_gateway.gateway));
				printf("templateID=%d's NMSIP=%s\n", template_req[i].ap_template.templateIndex, inet_ntoa(template_req[i].ap_template.AP_Server_IP));
				send_to_apServer(apServer_index, APSERVER_WEB_TEMPLATE_MODIFY, NULL, &template_req[i], NULL, 1, NULL);
				i++;
			}
		}
		/*
		template_a = Fill_apTemplate();
		for (template_b = 0; template_b < template_a; template_b++)
			{
			legalTemplate[template_b] = 1;
		}

		send_to_apServer(apServer_index, APSERVER_WEB_TEMPLATE_REPORT, NULL, &template_req[i], NULL, 1, NULL);*/
		mysql_free_result(result);
		break;

	case APSERVER_WEB_AP_REPORT:
		decode_ap_report(buf,&list[0]);
		//write AP to database
		for (i = 0; i < buf[1]; i++)
		{
			strcpy(ip1, inet_ntoa(list[i].configure.pool.start));
			strcpy(ip2, inet_ntoa(list[i].configure.pool.end));
			strcpy(ip3, inet_ntoa(list[i].configure.AP_IP));
			strcpy(ip4, inet_ntoa(list[i].configure.ap_gateway.gateway));
			strcpy(ip5, inet_ntoa(list[i].configure.AP_Server_IP));
			strcpy(ip6, inet_ntoa(list[i].configure.ap_gateway.subnetmask));
			snprintf(sql_insert, 512, "INSERT INTO AP (serials_ID, template_ID, SSID_name, SSID_psw, login_name, login_psw,\
			NATOrBride, pool_start, pool_end, dhcporstatic, ip_adress, gateway, APServer_IP, state, user_number, mode,version, antenna_number,netmask) \
			Values ('%s',%d,'%s','%s','%s','%s',%d,'%s','%s',%d,'%s','%s','%s',%d,%d,%d,'%s',%d,'%s')", \
				list[i].AP_SN, list[i].configure.templateIndex,list[i].configure.ap_ssid.ssid, list[i].configure.ap_ssid.ssid_psw, list[i].configure.ap_login.login,\
				list[i].configure.ap_login.login_psw,list[i].configure.NatOrBridge, ip1, ip2,list[i].configure.DHCPOrStatic, ip3, ip4, ip5,\
				list[i].status.online_state, list[i].status.user_number, list[i].status.model, list[i].status.version, list[i].status.antenna_number,ip6);
			printf("sql_insert is；%s\n", sql_insert);
			ret = mysql_query(conn, sql_insert);//将AP_Server的数据写入数据库中
			if (ret == 0)
			{	
				printf("AP_reporte insert success\n");
				update_legalAP_add_o(&list[i]);//更新ap_list
				//printf("mysql_query error is: %s\n", mysql_error(&mysql));
			}
			memset(ip1, 0, sizeof(ip1)); memset(ip2, 0, sizeof(ip2)); memset(ip3, 0, sizeof(ip3)); memset(ip4, 0, sizeof(ip4)); memset(ip5, 0, sizeof(ip5)); memset(ip6, 0, sizeof(ip6));
		}
			send_to_apServer(apServer_index, APSERVER_WEB_HEART_BEAT, NULL, NULL, NULL, 0, NULL);
			legalAPServerTimer[apServer_index].apServer_timer_flag = WEB_APSERVER_ON;
			legalAPServerTimer[apServer_index].heart_beat_timer = web_apServer_heart_time;
			legalAPServerTimer[apServer_index].heart_beat_timeout_times = 0;

			mysql_query(conn, "select * from AP");//返回数据给AP_server
			result = mysql_store_result(&mysql);
			if (result == NULL)
			{
				printf("mysql_store_result error: %s\n", mysql_error(&mysql));
			}
			else
			{
				while ((row = mysql_fetch_row(result)))
				{
					(&legalAP_req[i])->command = AP_ADD;
					strcpy((&legalAP_req[i])->AP_SN, &row[0][0]);
					(&legalAP_req[i])->ap_template.templateIndex = atoi(&row[1][0]);
					strcpy((&legalAP_req[i])->ap_template.ap_ssid.ssid, &row[2][0]);
					strcpy((&legalAP_req[i])->ap_template.ap_ssid.ssid_psw, &row[3][0]);
					strcpy((&legalAP_req[i])->ap_template.ap_login.login, &row[4][0]);
					strcpy((&legalAP_req[i])->ap_template.ap_login.login_psw, &row[5][0]);
					(&legalAP_req[i])->ap_template.NatOrBridge = atoi(&row[6][0]);
					(&legalAP_req[i])->ap_template.pool.start.s_addr = inet_addr(&row[7][0]);
					(&legalAP_req[i])->ap_template.pool.end.s_addr = inet_addr(&row[8][0]);
					(&legalAP_req[i])->ap_template.DHCPOrStatic = atoi(&row[9][0]);
					(&legalAP_req[i])->ap_template.AP_IP.s_addr = inet_addr(&row[10][0]);
					(&legalAP_req[i])->ap_template.ap_gateway.gateway.s_addr = inet_addr(&row[11][0]);
					(&legalAP_req[i])->ap_template.AP_Server_IP.s_addr = inet_addr(&row[12][0]);
					(&legalAP_req[i])->ap_template.ap_gateway.subnetmask.s_addr = inet_addr(&row[18][0]);
					printf("已将AP中的数据发送给AP_Server,AP_serial_ID为:%s\n", &row[0][0]);
					send_to_apServer(apServer_index, APSERVER_WEB_AP_MODIFY, NULL, NULL, &legalAP_req[i], 1, NULL);
					i++;
				}
			}
			/*
			ap_a = Fill_ap_list();
			for (ap_b = 0; ap_b < ap_a; ap_b++)
				{
				legalAP[ap_b] = 1;
			}
			send_to_apServer(apServer_index, APSERVER_WEB_AP_REPORT, NULL, NULL, &legalAP_req[i], 1, NULL);*/
			mysql_free_result(result);
			break;
	case APSERVER_WEB_TEMPLATE_MODIFY:
		decode_template_modify_report(buf);//解析apserver发送过来的更改模板确认信息

		//write result to database. update control set ack = %d where templateIndex = %d
		while (Template_Ack[i].ack == '0')
		{
			i++;
		}
		if (i == buf[1])
		{
			snprintf(sql_insert, 512, "update control set state = 2 where content like '%%%d%%'", Template_Ack[--i].templateIndex);//更新control表数据
			ret = mysql_query(conn, sql_insert);//将数据写入数据库中
			if (ret != 0) {
				printf("mysql_query error is: %s\n", mysql_error(&mysql));
			}
		}
		else
		{
			for (i = 0; i < buf[1]; i++)
			{
				temp[k] = Template_Ack[i].ack;
				k++;
				temp[k] = ',';
				k++;
			}
			temp[--k] = '\0';
			snprintf(sql_insert, 512, "update control set state = 5,content = '%s' where content like '%%%d%%'",temp,Template_Ack[--i].templateIndex);
			ret = mysql_query(conn, sql_insert);
			if (ret != 0)
			{
				printf("mysql_query error is:%s\n", mysql_error(&mysql));
			}
		}
		legalAPServerTimer[apServer_index].apServer_on_timer_flag = 0;
		break;
	case APSERVER_WEB_AP_MODIFY:
		decode_ap_modify_report(buf);
		//write result to database
		while (legalAP_Ack[i].ack == '0')
		{
			i++;
		}
		if (i == buf[1])
		{
			snprintf(sql_insert, 512, "update control set state = 2 where content like '%%%s%%'", legalAP_Ack[--i].AP_SN);
			ret = mysql_query(conn, sql_insert);//将数据写入数据库中
			if (ret != 0) 
			{
				printf("mysql_query error is: %s\n", mysql_error(&mysql));
			}
		}	
		else
		{
			for (i = 0; i < buf[1]; i++)
			{
				temp[k] = legalAP_Ack[i].ack;
				k++;
				temp[k] = ',';
				k++;
			}
			temp[--k] = '\0';
			snprintf(sql_insert, 512, "update control set state = 5,content = '%s' where content like '%%%s%%'",temp, legalAP_Ack[--i].AP_SN);
			ret = mysql_query(conn, sql_insert);
			if (ret != 0)
			{
				printf("mysql_query error is_1072:%s\n", mysql_error(&mysql));
			}
		}
		legalAPServerTimer[apServer_index].apServer_on_timer_flag = 0;
		break;
	case APSERVER_WEB_AP_CONFIGURE:
		decode_ap_configure_report(buf);
		printf("Receive ap:%s configure ack(result=%d) from ap server\n",AP_Configure_Ack[0].AP_SN,AP_Configure_Ack[0].ack);
		//write result to database
		mysql_query(conn, "select * from temporary");
		result1 = mysql_store_result(&mysql);
		if (result1 == NULL) {
			printf("mysql_store_result error: %s\n", mysql_error(&mysql));
			printf("sql_insert is:%s\n", sql_insert);
		}
		else
		{
			row1 = mysql_fetch_row(result1);
			strcpy((&apTemplate[0])->ap_ssid.ssid, &row1[0][0]);
			strcpy((&apTemplate[0])->ap_ssid.ssid_psw, &row1[1][0]);
			strcpy((&apTemplate[0])->ap_login.login, &row1[2][0]);
			strcpy((&apTemplate[0])->ap_login.login_psw, &row1[3][0]);
			(&apTemplate[0])->NatOrBridge = atoi(&row1[4][0]);
			(&apTemplate[0])->pool.start.s_addr = inet_addr(&row1[5][0]);
			(&apTemplate[0])->pool.end.s_addr = inet_addr(&row1[6][0]);
			(&apTemplate[0])->DHCPOrStatic = atoi(&row1[7][0]);
			(&apTemplate[0])->AP_IP.s_addr = inet_addr(&row1[8][0]);
			(&apTemplate[0])->ap_gateway.gateway.s_addr = inet_addr(&row1[9][0]);
			(&apTemplate[0])->AP_Server_IP.s_addr = inet_addr(&row1[10][0]);
			(&apTemplate[0])->ap_gateway.subnetmask.s_addr = inet_addr(&row1[11][0]);
			for (i = 0; i < buf[1]; i++)
			{
				if (AP_Configure_Ack[i].ack == 1)
				{
					snprintf(sql_insert, 512, "update AP set SSID_name='%s',SSID_psw='%s',login_name='%s',login_psw='%s', NATorBride=%d,pool_start='%s', pool_end='%s',\
				dhcporstatic=%d,ip_adress='%s',gateway='%s',APserver_IP='%s',netmask = '%s' where serials_ID = '%s'", (&apTemplate[0])->ap_ssid.ssid, (&apTemplate[0])->ap_ssid.ssid_psw,
						(&apTemplate[0])->ap_login.login, (&apTemplate[0])->ap_login.login_psw, (&apTemplate[0])->NatOrBridge, &row1[5][0], &row1[6][0], (&apTemplate[0])->DHCPOrStatic,
						&row1[8][0], &row1[9][0], &row1[10][0], &row1[11][0], AP_Configure_Ack[i].AP_SN);
					printf("Update AP configure sql_insert is；%s\n", sql_insert);
					ret = mysql_query(conn, sql_insert);//更新AP的配置
					if (ret == 0)
						printf("AP_temp_configure insert success\n");
				}
			}
		}
			for (i = 0; i < buf[1]; i++)
			{
				if (legalAP_Ack[i].ack == 1)
				{
					for (k = 0; k < max_ap; k++)
					{
						if (strcmp(legalAP_Ack[i].AP_SN, ap_list[k].AP_SN) == 0)
						{
							strcpy(ap_list[k].configure.ap_ssid.ssid, &row1[0][0]);
							strcpy(ap_list[k].configure.ap_ssid.ssid_psw, &row1[1][0]);
							strcpy(ap_list[k].configure.ap_login.login, &row1[2][0]);
							strcpy(ap_list[k].configure.ap_login.login_psw, &row1[3][0]);
							ap_list[k].configure.NatOrBridge = atoi(&row1[4][0]);
							ap_list[k].configure.pool.start.s_addr = inet_addr(&row1[5][0]);
							ap_list[k].configure.pool.end.s_addr = inet_addr(&row1[6][0]);
							ap_list[k].configure.DHCPOrStatic = atoi(&row1[7][0]);
							ap_list[k].configure.AP_IP.s_addr = inet_addr(&row1[8][0]);
							ap_list[k].configure.ap_gateway.gateway.s_addr = inet_addr(&row1[9][0]);
							ap_list[k].configure.AP_Server_IP.s_addr = inet_addr(&row1[10][0]);
							ap_list[k].configure.ap_gateway.subnetmask.s_addr = inet_addr(&row1[11][0]);
							update_legalAP_del(ap_list[k].AP_SN);
							update_legalAP_add_o(&ap_list[k]);//更新数组
						}
					}
				}
			}
			i = 0;
			mysql_free_result(result1);
			while (AP_Configure_Ack[i].ack == 1)
			{
				i++;
			}
			if (i == buf[1])
			{
				snprintf(sql_insert, 512, "update control set state = 2 where content like '%%%s%%'", AP_Configure_Ack[--i].AP_SN);
				ret = mysql_query(conn, sql_insert);//将确认信息写入control表
				if (ret != 0)
				{
					printf("mysql_query error is: %s\n", mysql_error(&mysql));
				}
			}
			else
			{
				for (i = 0; i < buf[1]; i++)
				{
					temp[k] = AP_Configure_Ack[i].ack;
					k++;
					temp[k] = ',';
					k++;
				}
				temp[--k] = '\0';
				snprintf(sql_insert, 512, "update control set state = 5,content = '%s' where content like '%%%s%%'", temp, AP_Configure_Ack[--i].AP_SN);
				ret = mysql_query(conn, sql_insert);
				if (ret != 0)
				{
					printf("mysql_update_result error is:%s\n", mysql_error(&mysql));
				}
			}
		legalAPServerTimer[apServer_index].apServer_on_timer_flag = 0;
		break;
	case APSERVER_WEB_HEART_BEAT:
		legalAPServerTimer[apServer_index].heart_beat_timer = web_apServer_heart_time;
		legalAPServerTimer[apServer_index].heart_beat_timeout_times = 0;
		
		decode_ap_status_reporter(buf, &list[0]);
		//state	user_number	mode	 char version	antenna_number

	   for (i = 0; i < buf[1]; i++)//更新status
		{
		   snprintf(sql_insert, 512, "update AP set state = %d,user_number = %d,mode = %d,version = '%s',antenna_number = %d where serials_ID = '%s'",
			   list[i].status.online_state, list[i].status.user_number, list[i].status.model, list[i].status.version, list[i].status.antenna_number,list[i].AP_SN);
		  ret =  mysql_query(conn, sql_insert);
		  if (ret == 0)
		  {
			  printf("update Ap：%s success!state=%d\n",list[i].AP_SN,list[i].status.online_state);
			  update_legalAP_del(list[i].AP_SN);
			  update_legalAP_add_o(&list[i]);
		  }
			
		}
		//ap_status_assign_value(&ap_list[apServer_index],pp);
		break;
	case APSERVER_WEB_SoftWare_Upgrade:
		legalAPServerTimer[apServer_index].apServer_timer_flag = WEB_APSERVER_SoftWare_Upgrade;
		legalAPServerTimer[apServer_index].apServer_on_timer = web_apServer_upgrade_time;
		decode_ap_upgrade_report(buf);
		//write result to database
		while (legalAP_upgrade_Ack[i].ack == '0')
		{
			i++;
		}
		if (i == buf[1])
		{
			snprintf(sql_insert, 512, "update control set state = 2 where content like '%%%s%%'", legalAP_upgrade_Ack[--i].AP_SN);
			ret = mysql_query(conn, sql_insert);//将数据写入数据库中
			if (ret != 0) 
			{
				printf("mysql_query error is: %s\n", mysql_error(&mysql));
			}
		}	
		else
		{
			
			for (j = 0; j < buf[1]; j++)
			{
                p1 = legalAP_upgrade_Ack[j].AP_SN;
			    p2 = str;
				while (*p1 != '\0') //  注意是'\0'
					p1++;

				while (*p2 != '\0')
				{
					*p1++ = *p2++; //已证明地址有改变
					printf("%p  ", p2);

				}
			}
			for (i = 0; i < buf[1]; i++)
			{
				memcpy(temp1 + (21 * i) , legalAP_upgrade_Ack[i].AP_SN, 21);
					temp[k] = AP_Configure_Ack[i].ack;
					k++;
					temp[k] = ',';
					k++;
			}
			temp[--k] = '\0';
			p3 = temp1;
			p4 = temp;
			while (*p3 != '\0') //  注意是'\0'
				p3++;

			while (*p4 != '\0')
			{
				*p3++ = *p4++; //已证明地址有改变
				printf("%p  ", p4);

			}
			snprintf(sql_insert, 512, "update control set state = 5,content = '%s' where content like '%%%s%%'",temp1, legalAP_upgrade_Ack[--i].AP_SN);
			ret = mysql_query(conn, sql_insert);
			if (ret != 0)
			{
				printf("mysql_query error is_1072:%s\n", mysql_error(&mysql));
			}
		}
		legalAPServerTimer[apServer_index].apServer_on_timer_flag = 0;
		break;
	case APSERVER_WEB_Request_LogInfo://第二个操作SCp
	    legalAPServerTimer[apServer_index].apServer_timer_flag = WEB_APSERVER_Request_LogInfo;
		legalAPServerTimer[apServer_index].apServer_on_timer = web_apServer_upgrade_time;
		decode_ap_upgrade_report(buf);//解析apserver发送过来的模板

		/*apTemplate[i].ap_ssid.ssid  apTemplate[i].ap_ssid.ssid_psw  apTemplate[i].ap_login.login  apTemplate[i].ap_login.login_psw  apTemplate[i].options
		apTemplate[i].DHCPOrStatic  apTemplate[i].NatOrBridge    apTemplate[i].pool.start  apTemplate[i].pool.end  apTemplate[i].ap_gateway.gateway变量*/
		strcpy(ip1,legalAP_upgrade_Ack[0].Scp_Username);
		strcpy(ip2,legalAP_upgrade_Ack[0].Scp_PassWord);
		strcpy(ip3,legalAP_upgrade_Ack[0].Remote_FilePath);
		while (legalAP_upgrade_Ack[i].ack == '0')
		{
			i++;
		}
		if (i == buf[1])
		{   
            for (j = 0; j < buf[1]; j++)
			{
                p1 = legalAP_upgrade_Ack[j].AP_SN;
			    p2 = str;
				while (*p1 != '\0') //  注意是'\0'
					p1++;

				while (*p2 != '\0')
				{
					*p1++ = *p2++; //已证明地址有改变
					printf("%p  ", p2);

				}
			}
			for (i = 0; i < buf[1]; i++)
			{
				memcpy(temp1+(21 * i) , legalAP_upgrade_Ack[i].AP_SN, 21);
					temp[k] = AP_Configure_Ack[i].ack;
					k++;
					temp[k] = ',';
					k++;
			}
			temp[k]='/';
			k = k + 1;
			temp[k]='v';
			k = k + 1;
			temp[k]='a';
			k = k + 1;
			temp[k]='r';
			k = k + 1;
			temp[k]='/';
			k = k + 1;
			temp[--k] = '\0';
			p3 = temp1;
			p4 = temp;
			while (*p3 != '\0') //  注意是'\0'
				p3++;

			while (*p4 != '\0')
			{
				*p3++ = *p4++; //已证明地址有改变
				printf("%p  ", p4);

			}
			p5 = temp1;
			p6 = ip3;
			while (*p5 != '\0') //  注意是'\0'
				p5++;

			while (*p6 != '\0')
			{
				*p5++ = *p6++; //已证明地址有改变
				printf("%p  ", p4);

			}
			snprintf(sql_insert, 512, "update control set state = 2,content = '%s' where content like '%%%s%%'",temp1, legalAP_upgrade_Ack[--i].AP_SN);
			snprintf(s, 512, "sshpass -p \"%s\" scp  -o StrictHostKeyChecking=no %s:%s /var/",ip2, ip1, ip3);//想插入的位置可按照自己意愿修改此处，且temp[k] 存的位置也修改
			status = system(s);
			if (-1 == status){ 
			   printf("scp error");
			}
			if (!WIFEXITED(status)){
				printf("scp error");
			}
			if (WEXITSTATUS(status)){
				printf("scp error");
			}
			else{
				printf("scp success");
			}//?要不要
			ret = mysql_query(conn, sql_insert);//将数据写入数据库中
			if (ret != 0) 
			{
				printf("mysql_query error is: %s\n", mysql_error(&mysql));
			}
		}	
		else
		{
			for (j = 0; j < buf[1]; j++)
			{
                p1 = legalAP_upgrade_Ack[j].AP_SN;
			    p2 = str;
				while (*p1 != '\0') //  注意是'\0'
					p1++;

				while (*p2 != '\0')
				{
					*p1++ = *p2++; //已证明地址有改变
					printf("%p  ", p2);

				}
			}
			for (i = 0; i < buf[1]; i++)
			{
				memcpy(temp1+(21 * i) , legalAP_upgrade_Ack[i].AP_SN, 21);
					temp[k] = AP_Configure_Ack[i].ack;
					k++;
					temp[k] = ',';
					k++;
			}
			temp[k]='/';
			k = k + 1;
			temp[k]='v';
			k = k + 1;
			temp[k]='a';
			k = k + 1;
			temp[k]='r';
			k = k + 1;
			temp[k]='/';
			k = k + 1;
			temp[--k] = '\0';
			p3 = temp1;
			p4 = temp;
			while (*p3 != '\0') //  注意是'\0'
				p3++;

			while (*p4 != '\0')
			{
				*p3++ = *p4++; //已证明地址有改变
				printf("%p  ", p4);

			}
			p5 = temp1;
			p6 = ip3;
			while (*p5 != '\0') //  注意是'\0'
				p5++;

			while (*p6 != '\0')
			{
				*p5++ = *p6++; //已证明地址有改变
				printf("%p  ", p4);

			}
			snprintf(sql_insert, 512, "update control set state = 5,content = '%s' where content like '%%%s%%'",temp1, legalAP_upgrade_Ack[--i].AP_SN);
			snprintf(s, 512, "scp %s  %s:/var/",ip3,ip1);
		    status = system(s);
			if (-1 == status){ 
			   printf("scp error");
			}
			if (!WIFEXITED(status)){
				printf("scp error");
			}
			if (WEXITSTATUS(status)){
				printf("scp error");
			}
			else{
				printf("scp success");
			}//？要不要 ？
			ret = mysql_query(conn, sql_insert);
			if (ret != 0)
			{
				printf("mysql_query error is_1072:%s\n", mysql_error(&mysql));
			}
		}
		legalAPServerTimer[apServer_index].apServer_on_timer_flag = 0;
		break;
	default:
		log_error("Error: Unknown command from APServer = %d!\n", buf[0]);
		break;
		}


	  //这里添加一个收信息更新数据库的操作
}

//AP_Configuration_template apTemplate[max_template];
//AP ap_list[max_ap];
//int legalTemplate[max_template];
//int legalAP[max_ap];
void Fill_APServer_socket_list()//将apserver的信息写入apServer_socket_list数组
{
	int i = 0;
	int ret = -1;
	ret = mysql_query(conn, "select * from ap_server");
	if (ret != 0) {
		printf("mysql_select error: %s\n", mysql_error(&mysql));
	}
	result = mysql_store_result(&mysql);
	if (result == NULL)
	{
		printf("mysql_store_result error: %s\n", mysql_error(&mysql));
	}
	while ((row = mysql_fetch_row(result)))
	{
		(&apServer_socket_list[i])->addrAP.sin_addr.s_addr = inet_addr(&row[1][0]);
		i++;
	}
	mysql_free_result(result);
}

void print_info(void)//打印信息，无关紧要
{
	int i = 0;
	printf("==========================================================\n");
	while ((&apServer_socket_list[i])->addrAP.sin_addr.s_addr)
	{
		printf("AP_server_list:%s\n", inet_ntoa((&apServer_socket_list[i])->addrAP.sin_addr));
		i++;
	}
	i = 0;
	printf("==========================================================\n");
	for (i = 0; i < 8; i++)
	{
		printf("apTemplate:tenplateIndex,ap_ssid:%d, %s\n", apTemplate[i].templateIndex, apTemplate[i].ap_ssid.ssid);

	}
	i = 0;
	printf("==========================================================\n");
	for (i = 0; i < 8; i++)
	{
		printf("ap_list:AP_SN,apTemplate:%s, %d\n", ap_list[i].AP_SN, ap_list[i].configure.templateIndex);
	
	}
}

void update_legalTemplate_del(int index)//删除模板后，更新数组apTemplate
{
	int i= 0;
	for (i = 0; i < max_template; i++)
	{
		if (apTemplate[i].templateIndex == index)
		{
			break;
		}
	}
	memset(&apTemplate[i], 0, sizeof(AP_Configuration_template));
	legalTemplate[i] = 0;
}//AP_Configuration_template* apTemplate 

void update_legalTemplate_add(template_operation_req *template_req)//添加模板后，更新数组apTemplate
{
	int i= 0;
		for (i = 0; i < max_template; i++)
		{
			if (legalTemplate[i] == 0)
				break;
		}
		legalTemplate[i] = 1;
		template_assign_value(&apTemplate[i], (&template_req->ap_template));
}

void update_legalTemplate_add_o(AP_Configuration_template* ap_Template)//删除模板后，更新数组apTemplate
{
	int i= 0;
	for (i = 0; i < max_template; i++)
	{
		if (legalTemplate[i] == 0)
			break;
	}
	legalTemplate[i] = 1;
	template_assign_value(&apTemplate[i],ap_Template);
}

void update_legalAP_del(char* index)//删除ap后，更新数组ap_list
{
	int i= 0;
	for (i = 0; i < max_ap; i++)
	{
		if (strcmp(ap_list[i].AP_SN,index) == 0)
		{
			break;
		}
	}
	memset(&ap_list[i], 0, sizeof(AP));
	legalAP[i] = 0;
}
void update_legalAP_add(legalAP_operation_req* AP_req)
{
	int i= 0;
	for (i = 0; i < max_ap; i++)
	{
		if (legalAP[i] == 0)
			break;
	}
	legalAP[i] = 1;
	strcpy(ap_list[i].AP_SN , AP_req->AP_SN);
	template_assign_value((&ap_list[i].configure),(&AP_req->ap_template));
}
void update_legalAP_add_o(AP* AP_list)
{//添加ap后，更新数组ap_list
	int i= 0;
	for (i = 0; i < max_ap; i++)
	{
		if (legalAP[i] == 0)
			break;
	}
	legalAP[i] = 1;
	strcpy(ap_list[i].AP_SN,AP_list->AP_SN);
	ap_configure_assign_value(&ap_list[i], AP_list);
	ap_status_assign_value(&ap_list[i], AP_list);
}