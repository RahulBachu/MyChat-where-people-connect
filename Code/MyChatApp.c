/* MAIN MYCHAT APPLICATION UTIL FILE */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "client.c"

typedef struct Details
{
	char *data;
	struct Details *next;
}Details;

Details *id = NULL;
Details *ip = NULL;
Details *password = NULL;
Details *username = NULL;

int assertLogin=0, choice=0;

char  *currentPassword=NULL, *currentUserID=NULL, *mode=NULL;

Details *insertAccount(Details *details,char *key)
{
	Details *temp = (Details *)malloc(sizeof(Details));
	temp->data = (char *)malloc(256*sizeof(char));
	strcpy(temp->data,key);
	temp->next = NULL;
	if(details == NULL)
	{
		details = temp;
	}else{
		temp->next = details;
		details=temp;
	}
	return details;
}

//function to retrive details from file in to0 respective linked list
Details *loadAcc(int n,Details *details)
{
	FILE *fp;
	fp=fopen("Details.txt","r");
	if(!fp)
	{
	    return NULL;
	}
	if(details==NULL)
	{
		int k,i=1,sum=0;
		char *buffer = (char *)malloc(256*sizeof(char));
		switch(n)
		{
			case 1: k=2;
					break;
			case 2: k=3;
					break;
			case 3: k=4;
					break;
			case 4: k=5;
					break;
		}
		
		while(fgets(buffer,256,fp) != NULL)
		{
			if(i==k)
			{
				sum=i;
			}
			if(sum==i)
			{
				buffer[strcspn(buffer,"\n")]='\0';
				details=insertAccount(details,buffer);
				sum=i+5;
			}
			i++;
		}
	}
	fclose(fp);
	return details;
}

int validateUserID(char *userid)
{
	Details *temp = id;
	int i,flag=0;
	if(temp == NULL)
		return 1;
	while (temp != NULL)
	{
		flag=0;
		for(i=0;i<strlen(temp->data)-1;i++)
		{
			if(temp->data[i]!=userid[i]){
				flag=1;
				break;
			}
		}
		if(flag==0)
			return 0;
		temp=temp->next;
	}
	int small=0,capital=0,u=0,digits=0;
	while(*userid)
	{
		if(*userid >= 'a' && *userid <= 'z')
		{
			small++;	
		}else if(*userid >= 'A' && *userid <= 'Z')
		{
			capital++;		
		}else if(*userid == '_' )
		{
			u++;		
		}else if(*userid <= '9' && *userid >= '0')
		{
			digits++;
		}else{
			return 0;
		}
		userid++;
	}
	if(small!=0 && capital!=0 && u!=0)
		return 1;
	else
		return 0;
}

int validatePassword(char *pwd)
{
	int small=0,capital=0,special=0,digits=0;
	while(*pwd)
	{
		if(*pwd >= 'a' && *pwd <= 'z')
		{
			small++;	
		}else if(*pwd== '&' || *pwd == '#' || *pwd == '$' || *pwd == '%' || *pwd == '@')
		{
			special++;
		}else if(*pwd >= 'A' && *pwd <= 'Z')
		{
			capital++;		
		}else if(*pwd <= '9' && *pwd >= '0')
		{
			digits++;
		}else{
			return 0;
		}
		pwd++;
	}
	if (small != 0 && capital != 0 && digits != 0 && special != 0){
    	return 1;
	}
  	else{
    	return 0;
  	}
}

//function to validate ip address 
int validateIP4(char *str)
{   
	int segs = 0,chcnt = 0,accum = 0;   // Segment count,Character count within segment,Accumulator for segment
    // Catch NULL pointer
    if (str == NULL)
        return 0;

    /* Process every character in string. */

    while (*str != '\0') {
        /* Segment changeover. */

        if (*str == '.') {
            /* Must have some digits in segment. */

            if (chcnt == 0)
                return 0;

            /* Limit number of segments. */

            if (++segs == 4)
                return 0;

            /* Reset segment values and restart loop. */

            chcnt = accum = 0;
            str++;
            continue;
        }
        /* Check numeric. */

        if ((*str < '0') || (*str > '9'))
            return 0;

        /* Accumulate and check segment. */

        if ((accum = accum * 10 + *str - '0') > 255)
            return 0;

        /* Advance other segment specific stuff and continue loop. */

        chcnt++;
        str++;
    }

    /* Check enough segments and enough characters in last segment. */

    if (segs != 3)
        return 0;

    if (chcnt == 0)
        return 0;

    return 1;
}

//display contents specified
void display(Details *details)
{
	Details *temp =details;
	if(temp== NULL)
		return;
	while(temp != NULL)
	{
		puts(temp->data);
		temp = temp->next;
	}
}

//function to invoke loadAcc function to load data from file
void LoadData()
{
	username=loadAcc(1,username);
	id=loadAcc(2,id);
	password=loadAcc(3,password);
	ip=loadAcc(4,ip);
}

//common function for all use cases
void ManualInfo(char *fname)
{
	FILE *fp;
	char c;
	fp=fopen(fname,"r");
	if(!fp){
		printf("\n\tUnable to open file assosciated\n\n");
		exit(0);
	}
	while((c=fgetc(fp)) != EOF)
	{
		printf("%c",c);
	}
}

//Module 1 - Registration
void Register()
{
	printf("\n\n\t^^^^^^^^^^^^^^^^^^^^^^^^  Registration Section  ^^^^^^^^^^^^^^^^^^^^^^^^\n\n\tTo Complete Registration. Fill in the details please!!\n\n");
	ManualInfo("RegiInfo.txt");
	printf("\n\n");
	FILE *fp;
	int i;
	char *password_1, *password_2, *fullname, *ipaddr, *userID,ch,temp;

	password_1=(char *)malloc(256*sizeof(char));
	password_2=(char *)malloc(256*sizeof(char));
	fullname=(char *)malloc(256*sizeof(char));
	ipaddr=(char *)malloc(256*sizeof(char));
	userID=(char *)malloc(256*sizeof(char));

	fp = fopen("Details.txt", "a");
	if(!fp)
		printf("Registration failed due to file not found error!\n\n");
	fputs("**********\n",fp);

	//user name details
	printf("\n\n\t<<.>> Enter Your Full Name : ");
	scanf("%c",&temp);
	scanf("%[^\n]",fullname);
	fprintf(fp,"%s\n",fullname);
	username=insertAccount(username,fullname);
	free(fullname);

	//userid details
	printf("\n\n\t<<.>> Enter User ID : ");
	scanf("%c",&temp);
	scanf("%[^\n]",userID);

	if(validateUserID(userID)){
		fprintf(fp,"%s\n",userID);
	}
	else{
		while(!validateUserID(userID))
		{
			printf("\n\t\tTry again!!\n");
			printf("\n\n\t<<.>> Enetr User ID : ");
			scanf("%c",&temp);
			scanf("%[^\n]",userID);
			
		}
		fprintf(fp,"%s\n",userID);
	}
	id=insertAccount(id,userID);
	free(userID);

	//password details
	printf("\n\n\t<<.>> Enter Password : ");
	scanf("%c",&temp);
	scanf("%[^\n]",password_1);

	printf("\n\n\t<<.>> Confirm Password : ");
	scanf("%c",&temp);
	scanf("%[^\n]",password_2);

	if((strlen(password_1)>=8 && strlen(password_1)<=20) && (strcmp(password_1, password_2) == 0) && (validatePassword (password_1)) && (validatePassword (password_2)))
    {
      	fprintf (fp, "%s\n", password_1);
    }
  	else
    {
   		while ((strlen(password_2)<8) || (strlen(password_2)>20) || strlen(password_1)<8 || strlen(password_1)>20 || (strcmp (password_1, password_2) != 0) || (!validatePassword (password_1)) || (!validatePassword (password_2)))
		{
			printf("\n\nTry again!!\n");
	 		printf ("\n\n\t<<.>> Enter Password : ");
	 		scanf("%c",&temp);
	 		scanf("%[^\n]",password_1);
	  		printf ("\n\n\t<<.>> Confirm Password : ");
	  		scanf("%c",&temp);
	  		scanf("%[^\n]",password_2);
		}
      	fprintf (fp, "%s\n", password_1);
    }
    password=insertAccount(password,password_1);
    free(password_1);
	free(password_2);

	printf("\n\n\t------ IP Address Section ------\n");
	printf("\n\n\tYour System's IP Address is shown below.\n\tUnder Row as ''Wireless LAN adapter Wi-Fi: '' into the 3rd Row as ''IPv4 Address. . . .''\n");
	
	printf("\n\n");
	for(i=0;i<30;i++)
		printf("*");
	printf("\n\n");
	system("c:\\windows\\system32\\ipconfig");
	printf("\n\n");
	for(i=0;i<30;i++)
		printf("*");
	printf("\n\n");

	printf("\n\n\t<<.>> Enter your IP address : ");
	scanf("%c",&temp);
	scanf("%[^\n]",ipaddr);

	if(validateIP4(ipaddr)){
		fprintf(fp,"%s\n",ipaddr);
	}
	else{
		while(!validateIP4(ipaddr))
		{
			printf("\n\n\tTry again!!\n");
			printf("\n\n\t<<.>> Enter IP Address : ");
			scanf("%c",&temp);
			scanf("%[^\n]",ipaddr);
			//scanf("%s",ipaddr);
		}
		fprintf(fp,"%s\n",ipaddr);
	}
	ip=insertAccount(ip,ipaddr);
	free(ipaddr);

	printf("\n\n\tBoomm...Your were Successfully Registered!!\n\n");
	fclose(fp);
}

//Module 2 - Login

int checkUL(Details *details,char *str)
{
	Details *temp =details;
	int i,flag;
	while(temp != NULL)
	{
		flag=0;
		for(i=0;i<strlen(temp->data);i++)
		{
			if(temp->data[i]!=str[i])
			{
				flag=1;
				break;
			}
		}
		if(flag==0)
			return 1;
		temp=temp->next;
	}
	return 0;
}

int printpassword(char *name,char *uid)
{
	Details *temp1=username, *temp2=password,*temp3=id;
	int i,flag=0,flag1=0;
	if(temp1==NULL)
		return 0;
	while(temp1 != NULL )
	{
		flag=0;
		for(i=0;i<strlen(temp1->data);i++)
		{
			if(temp1->data[i]!=name[i])
			{
				flag=1;
				break;
			}
		}
		if(flag==0)
		{
			temp3=id;
			temp2=password;
			while(temp3 && temp2)
			{
				flag1=0;
				for(i=0;i<strlen(temp3->data);i++)
				{
					if(temp3->data[i]!=uid[i])
					{
						flag1=1;
						break;
					}
				}
				if(flag1==0)
				{
					printf("\n\n\t<<<.>>> Password : %s \n\n",temp2->data);
					return 1;
				}
				temp3=temp3->next;
				temp2=temp2->next;
			}
		}
		temp1=temp1->next;
		temp2=temp2->next;
	}
	printf("\n\n\tPlease Enter valid Registered name assosciated with user ID %s, if not Register your self, then try logging in..!!\n\n",uid);
	return 0;
}

void UserLogin()
{
	char *uID, *pwd,temp;
	int ch,i=0;
	uID = (char *)malloc(256*sizeof(char));
	pwd = (char *)malloc(256*sizeof(char));
	currentUserID=(char *)malloc(256*sizeof(char));
	currentPassword=(char *)malloc(256*sizeof(char));
	
	system("cls");
	if(assertLogin)
	{
		printf("\n\n\tYou are already logged in !!\n\n\tIf you want to login with a different account, please Log Out from existing account\n\n");
		return;
	}
	assertLogin=0;
	printf("\n\n\t ^^^^^^^^^^ LOGIN SECTION ^^^^^^^^ \n\n");
	printf("\n\n\t<<.>> Enter User ID : ");
	scanf("%c",&temp);
	scanf("%[^\n]",uID);
	if(!checkUL(id,uID))
	{
		printf("\n\n\tIf not registered enter '0' else any numbers for proceeding with login.");
		printf("\n\n\t<<.>> Enter here : ");
		scanf("%d",&ch);
		if(ch==0){
			Register();
		}
		while(!checkUL(id,uID))
		{
			printf("\n\n\tNow give a try..\n");
			printf("\n\t<<.>> Enter User ID : ");
			scanf("%c",&temp);
			scanf("%[^\n]",uID);
		}
	}
	uID[strcspn(uID,"\n")]='\0';
	printf("\n\n\t<<.>> Enter Password : ");
	scanf("%c",&temp);
	scanf("%[^\n]",pwd);
	if(!checkUL(password,pwd))
	{
		printf("\n\n\tEnter Password Correctly!!, If you have forgotten your password, please enter '1' else to continue enter any number.\n\n\t::> ");
		scanf("%d",&ch);
		i=0;
		while(ch==1&& i!=1){
			printf("\n\n\t<<.>> Enter your Registered Full Name : ");
			scanf("%c",&temp);
			scanf("%[^\n]",pwd);
			i=printpassword(pwd,uID);
		}
		while(!checkUL(password,pwd))
		{
			printf("\n\n\tNow Try Again\n");
			printf("\n\n\t<<.>> Enter Password : ");
			scanf("%c",&temp);
			scanf("%[^\n]",pwd);
		}
	}
	assertLogin=1;
	strcpy(currentUserID,uID);
	strcpy(currentPassword,pwd);
	printf("\n\n\tYou were successfully authenticated and logged in !!\n\n");
	free(uID);
	free(pwd);
}
//--------------
void UserLogout()
{
	if(assertLogin!=0)
	{
		assertLogin=0;
		printf("\n\n\tYou Were successfully logged out!!\n\n");
		currentUserID=NULL;
		currentPassword=NULL;
		return;
	}else{
		printf("\n\n\tYou are not logged in!!\n");
		printf("\n\tTo login, Enter '1' for yes or any number to go back to main menu.\n\n\t::> ");
		int ch;
		scanf("%d",&ch);
		if(ch==1)
		{
			UserLogin();
		}else
			return;
	}
}
//-----------------------
int ReLoadFile()
{
	Details *temp1=id,*temp2=ip,*temp3=password,*temp4=username;
	FILE *fp=fopen("Details.txt","w");
	if(fp==NULL)
	{
		printf("\n\n\t Reload Error : 'Unable to open file'\n");
		return 0;
	}
	if(temp1==NULL || temp2==NULL || temp3==NULL || temp4==NULL)
	{
		fclose(fp);
		return 1;
	}else
	{
		while(temp1!=NULL && temp2!=NULL && temp3!=NULL && temp4!=NULL)
		{
			fprintf(fp,"**********\n");
			fprintf(fp,"%s\n",temp4->data);
			fprintf(fp,"%s\n",temp1->data);
			fprintf(fp,"%s\n",temp3->data);
			fprintf(fp,"%s\n",temp2->data);
			temp1=temp1->next;
			temp2=temp2->next;
			temp3=temp3->next;
			temp4=temp4->next;
		}
		fclose(fp);
		printf("\n\n");
		return 1;
	}
}

//-------------- delete module --------------------------------
static int found=0;
Details* deleteUserDetail(Details *details,char *str)
{
	Details *temp;
	if(details==NULL){
		return details;
	}else
	 {
	 	found++;
	 	if(strncmp(str,details->data,strlen(str)-1)==0)
		{
			temp=details->next;
			return temp;
		}else{
			details->next=deleteUserDetail(details->next,str);
			return details;
		}
	}
}

Details* deleteAtDetail(Details *details, int pos)
{
	Details *temp=details,*prev,*del;
	int i=0;
	if(details==NULL){
		return details;
	}
	else{
		if(details != NULL && pos == 1){
			details=details->next;
			return details;
		}
		else{
			while(i!=pos-2)
			{
				if(temp==NULL)
				{
					return details;
				}
				temp=temp->next;
				i++;
			}
			temp->next=temp->next->next;
			return details;
		}
	}
}

void DeleteAccount()
{
	char *str = (char *)malloc(256*sizeof(char));
	char temp;
	system("cls");
	printf("\n\n\t\t------ DELETE USER'S ACCOUNT SECTION ------\n");
	printf("\n\n\tEnter the User ID for deleting corresponding account.\n\n\t::> ");
	scanf("%c",&temp);
	scanf("%[^\n]",str);
	while(!checkUL(id,str))
	{
		printf("\n\n\tUser Not Found!!, Please Enter User ID Again Correctly. \n\n\t::> ");
		scanf("%c",&temp);
		scanf("%[^\n]",str);
	}

	id=deleteUserDetail(id,str);
	password=deleteAtDetail(password,found);
	ip=deleteAtDetail(ip,found);
	username=deleteAtDetail(username,found);
	if(assertLogin)
	{
		printf("\n\n\tYou were safely logged out!!\n\n");
		assertLogin=0;
	}
	printf("\n\n\tSuccessfully deleted account associated with userID ::> %s\n\n", str);
	found=0;
	if(!ReLoadFile())
	{
		printf("\n\n\tDelete Account Failed... | -_- |\n\n");
	}
	free(str);
}
//--------------- update password  module --------------------------------
Details* updateDetails(Details *details, char *new, char *old)
{
	Details *temp = details;
	while(temp != NULL)
	{
		if(strncmp(temp->data,old,strlen(old)-1)==0)
		{
			strcpy(temp->data,new);
			return details;
		}
		temp=temp->next;
	}
}

void changePassword()
{
	char *oldpwd,*newpwd1,*newpwd2,temp;
	int ch;
	oldpwd=(char *)malloc(256*sizeof(char));
	newpwd1=(char *)malloc(256*sizeof(char));
	newpwd2=(char *)malloc(256*sizeof(char));

	system("cls");
	if(!assertLogin)
	{
		printf("\n\n\tIn Order to Access this Functionality you need to be Logged inn. Kindly Log in.\n\n");
		return;
	}
	printf("\n\n\t\t------ CHANGE USER ACCOUNT'S PASSWORD SECTION ------\n");
	printf("\n\n\t<<.>> Enter Your Account associated Old Password : ");
	scanf("%c",&temp);
	scanf("%[^\n]",oldpwd);
	while(!strcmp(currentPassword,oldpwd)==0)
	{
		printf("\n\n\t<<.>> Please enter your old password Correctly : ");
		scanf("%c",&temp);
		scanf("%[^\n]",oldpwd);
	}
	printf("\n\n\t<<.>> Enter New Password : ");
	scanf("%c",&temp);
	scanf("%[^\n]",newpwd1);
	printf("\n\n\t<<.>> Enter New Password Again : ");
	scanf("%c",&temp);
	scanf("%[^\n]",newpwd2);
	while(!(strcmp(newpwd1,newpwd2) == 0) || !validatePassword(newpwd1) || !validatePassword(newpwd1))
	{
		printf("\n\n\t<<.>> Enter New Password : ");
		scanf("%c",&temp);
		scanf("%[^\n]",newpwd1);
		printf("\n\n\t<<.>> Enter New Password Again : ");
		scanf("%c",&temp);
		scanf("%[^\n]",newpwd2);
	}
	password=updateDetails(password,newpwd1,oldpwd);
	free(newpwd1);
	free(newpwd2);
	free(oldpwd);
	printf("\n\n\tSuccessfully Updated New Password\n\n\tPlease Login Again With New Password\n\n");
	assertLogin=0;
	if(!ReLoadFile())
	{
		printf("\n\n\tPassword Change Failed... | -_- |\n\n");
	}
	printf("\n\n\tYou were Logged Out due to chnages in password.\n\tEnter 1 - Log In, Any Number - To go back to main menu.\n\n\t::> ");
	scanf("%d",&ch);
	if(ch==1){
		UserLogin();
	}else
	{
		return;
	}
}

void chooseMode()
{
	int flag0=0;
	choice=0;
	printf("\n\n\tHow would you like to start this application?\n");
	while(flag0!=1)
	{
		printf("\n\t<< 1 >> Server Mode\t\t<< 2 >> Client Mode\n\t::> ");
		scanf("%d",&choice);
		switch (choice)
		{
			case 1:	flag0=1;
					break;
			case 2:	flag0=1;
					break;
			default:printf("\nInvalid choice. Enter again!\n");
					break;
		}
	}
	if(choice==1)
		mode = "Start App Server";
	else
		mode = "Connect to Server";
}

int main()
{
	system("cls");
	int flag1=0,flag2=0,flag3=0,ch,s1,s2,s3;
	LoadData();//loading previously saved data from file to respective linked list
	printf("\n\n\tIf Your are new to this application and, want to know how to use it..,\n\tthen Enter '1' for Yes and any number for No\n\t::> ");
	scanf("%d",&ch);
	if(ch==1)
	{
		printf("\n\n");
		ManualInfo("UserManual.txt");
	}
	chooseMode();

	printf("\n\t\t---->>> We Are Glad You Choose For Our Service!!..:XD  <<<----\n\n");
	sleep(1);
	printf("\nStarting Application.........\n");
	sleep(1);
	system("cls");
	while(flag1 != 1)
	{	
		flag2=0;
		printf("\n\n\t\t MyChat Main Menu\n\n");
		printf("\n\n\t<< 1 >> Register  \n\n\t<< 2 >> Login  \n\n\t<< 3 >> %s  \n\n\t<< 4 >> Delete Account  \n\n\t<< 5 >> Change Password  \n\n\t<< 6 >> Logout \n\n\t<< 7 >> Change Mode \n\n\t<< 8 >> Exit Application\n\n\t::> ",mode);
		scanf("%d", &s1);
		switch (s1)
		{
			case 1: Register();
					break;
			case 2: UserLogin();
					break;
			case 3: if(!assertLogin){
					printf("\n\n\tIn Order to Access this Functionality you need to be Logged inn. Kindly Log in.\n\n");
					break;
					}else{
					if(choice==1)
					{
					while(flag2!=1)
					{	system("cls");
						flag3=0;
						printf("\n\n\t\t---- Server Mode ---- \n");
						printf("\n\n\n\t<< 1 >> Start Conversation  \n\n\t<< 2 >> Send Data \n\n\t<< 3 >> Exit\n\n\t::> ");
						scanf("%d",&s2);
						switch(s2)
						{
							case 1:	while(flag3!=1)
									{
										printf("\n\n\t\t---- Conversation Section ---- \n");
										printf("\n\n\n\t<< 1 >> Group Conversation  \n\n\t<< 2 >> Personal Conversation \n\n\t<< 3 >> Exit\n\n\t::> ");
										scanf("%d",&s3);
										switch (s3)
										{
											case 1:	startServer(currentUserID,'G');
													break;
											case 2:	startServer(currentUserID,'P');
													break;
											case 3:	flag3=1;
													break;
											default:printf("\n\tInvalid choice !!\n");
													break;
										}
									}
									break;
							case 2:	startServer(currentUserID,'T');
									break;
							case 3:	flag2=1;
									break;
							default:printf("\n\tInvalid choice !\n");
									break;
						}
					}
					}
					else
					{
						while(flag2!=1)
						{	system("cls");
							flag3=0;
							printf("\n\n\t\t---- Client Mode ---- \n\n");
							printf("\n\t<< 1 >> Start Conversation  \n\n\t<< 2 >> Recieve Data \n\n\t<< 3 >> Exit\n\n\t::> ");
							scanf("%d",&s2);
							switch(s2)
							{
								case 1:	while(flag3!=1)
										{	
											printf("\n\n\t\t---- Conversation Section ---- \n\n");
											printf("\n\t<< 1 >> Group Conversation  \n\n\t<< 2 >> Personal Conversation \n\n\t<< 3 >> Exit\n\n\t::> ");
											scanf("%d",&s3);
											switch (s3)
											{
												case 1:	connect_to_Server(currentUserID,'G');
														break;
												case 2:	connect_to_Server(currentUserID,'P');
														break;
												case 3:	flag3=1;
														break;
												default:printf("Invalid choice !!");
														break;
											}
										}
										break;
								case 2:	connect_to_Server(currentUserID,'T');
										break;
								case 3:	flag2=1;
										break;
								default:printf("Invalid choice !");
										break;
							}
						}
					}}
					break;
			case 4: DeleteAccount();
					break;
			case 5: changePassword();
					break;
			case 6: UserLogout();
					break;
			case 7:	chooseMode();
					break;
			case 8:	flag1=1;
					break;
			default:printf("Invalid Operation. Try again!!\n\n");
					break;
		}
	}
	return 0;
}
