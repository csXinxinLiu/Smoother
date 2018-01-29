#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <string.h>
#include <string>
#include<queue>  
#include <vector>
#include <functional>
#include <time.h>
using namespace std;

#define SYSTEM_START_TIME 1
#define SYSTEM_END_TIME 7*1440
#define INTERVAL_TIME 10

int currentTime=SYSTEM_START_TIME;





class Job
{
public:
	Job(){}
	
	Job(int jN,int sT,int rT,int dl,float pD)
	{
		jobNumber=jN;
		submitTime=sT;
		runTime=rT;
		deadline=dl;
		slackTime=deadline-runTime-submitTime+1;
		powerDemand=pD;
	}
	Job(const Job &j)
	{
		jobNumber=j.jobNumber;
		submitTime=j.submitTime;
		runTime=j.runTime;
		deadline=j.deadline;
		slackTime=j.slackTime;
		powerDemand=j.powerDemand;
	}

	void subSlackTime(int subtime)
	{
		slackTime=slackTime-subtime;
	}


public:
	int jobNumber;
	int submitTime;
	int runTime;
	int deadline;
	int slackTime;
	double powerDemand;
};

bool operator< (const Job &j1,const Job &j2)  
{  
	return j2.slackTime < j1.slackTime;  
}

priority_queue<Job> queueJob;
vector<Job> incomingJob;


class JobScheduled
{
public:
	JobScheduled(Job j,int sT)
	{
		job=j;
		startTime=sT;
	}

	bool operator< (const JobScheduled &j1)const  
	{  
		return startTime < j1.startTime;  
	}

public:
	
	int startTime;
	Job job;
	
};
vector<JobScheduled> scheduledJob;


struct GreenEnergy
{
	GreenEnergy(){
		time=0;
		power=0.0;
	}
	GreenEnergy(int t,double p){
		time=t;
		power=p;
	}
	int time;
	double power;
	
};
vector<GreenEnergy> greenEnergy;

struct Grid
{
	Grid(){
		time=0;
		power=0.0;
	}
	Grid(int t,double p){
		time=t;
		power=p;
	}
	int time;
	double power;
	
};
vector<Grid> gridPower;

//float getPerSecondPowerOfRunningJob()
//{
//	float totalPower=0;
//	vector<Job>::iterator iter;
//	for (iter=runJob.begin();iter!=runJob.end();iter++)
//	{
//		totalPower=totalPower+iter->powerDemand;
//		if (iter->remRunTime!=1)
//		{
//			iter->updateRemainRunTime(1);
//		}else
//		{
//			runJob.erase(iter);
//		}
//
//	}
//	return totalPower;
//}

//float getPerSecondRemainPowerOfGreenEnergy(GreenEnergy greenPower,int time)
//{
//	float demandPower=getPerSecondPowerOfRunningJob();
//	float remainPower=greenPower.power-demandPower;
//	return remainPower;
//}

float updateRemainPowerOfGreenEnergyNewJob(Job &newJob,int time)
{
	
	double remianPower=greenEnergy[time-1].power-newJob.powerDemand;
	//newJob.updateRemainRunTime(1);
	if (remianPower<=0)
	{
		greenEnergy[time-1].power=0.0;
		gridPower[time-1].power=abs(remianPower);
	}else
	{
		greenEnergy[time-1].power=remianPower;
		gridPower[time-1].power=0.0;
	}
	return remianPower;
}




//float getTotalPowerOfRunningJob(vector<Job> &runJob,int interval)
//{
//	float totalPower=0;
//	vector<Job>::iterator iter;
//	for (iter=runJob.begin();iter!=runJob.end();iter++)
//	{
//		if (iter->remRunTime>interval)
//		{
//			totalPower=totalPower+iter->powerDemand*interval;
//			iter->updateRemainRunTime(interval);
//		}else
//		{
//			totalPower=totalPower+iter->powerDemand*iter->remRunTime;
//			runJob.erase(iter);
//		}
//		
//	}
//	return totalPower;
//}


//float getTotalPowerOfPowerVector(vector<float> &remainPower)
//{
//	float totalRemainPower=0;
//	vector<float>::iterator iter;
//
//	for (iter=remainPower.begin();iter!=remainPower.end();iter++)
//	{
//		if (*iter<=0)
//		{
//			totalRemainPower=totalRemainPower+0;
//		}
//		else
//		{
//			totalRemainPower=totalRemainPower+*iter;
//		}
//	}
//	return totalRemainPower;
//}


float getGreenPowerUsedWhenStartAtTime(Job &job,int time)
{
	
	float greenPowerUsed=0;
	for (int i=0;i<job.runTime;++i)
	{
		if (greenEnergy[time+i-1].power>job.powerDemand)
		{
			greenPowerUsed=greenPowerUsed+job.powerDemand;
		}else
		{
			greenPowerUsed=greenPowerUsed+greenEnergy[time+i-1].power;
		}
	}
	return greenPowerUsed;
}

int getStartTimeOfJobForMaxmizeGreenPower(Job &job)
{
	int time=currentTime;
	float maxUsed=0;
	int maxStartTime=time;

	for(;time<=(job.deadline-job.runTime)+1 && (time+job.runTime-1)<SYSTEM_END_TIME;time+=1) 
	{
		float used=getGreenPowerUsedWhenStartAtTime(job,time);
		if (used>maxUsed)
		{
			maxUsed=used;
			maxStartTime=time;
		}
	}
	return maxStartTime;
}


void updataSlackTimeOfQueueJob()
{
	for (int i=0;i<queueJob.size();++i)
	{

	}
}

void ScheduleJob()
{
	while (queueJob.empty()==false && queueJob.top().slackTime==0)
	{		
		Job prepareJob=queueJob.top();
		queueJob.pop();
		for (int i=0;i<prepareJob.runTime;++i)
		{
			updateRemainPowerOfGreenEnergyNewJob(prepareJob,currentTime+i);
		}
		JobScheduled sJob(prepareJob,currentTime);
		
		scheduledJob.push_back(sJob);
	}
		

	//float totalRemainPower=getTotalPowerOfPowerVector(remainPower);

	while (queueJob.empty()==false)
	{
		Job schJob=queueJob.top();
		int sTime=getStartTimeOfJobForMaxmizeGreenPower(schJob);
		for (int i=0;i<schJob.runTime;++i)
		{
			updateRemainPowerOfGreenEnergyNewJob(schJob,sTime+i);
		}
		queueJob.pop();

		JobScheduled sJob(schJob,sTime);
		scheduledJob.push_back(sJob);
		/*JobScheduled jobsch(schJob,sTime);
		queueScheduledJob.push(jobsch);*/
	}
	return;
}



int main()
{
	string name="Power_LANL_CM5_5436_8701";
	string fname=name+".csv";
	string fname1="greenPower-05-91011-smooth.csv";
	string wname="Result_smooth_"+name;
	wname+=".csv";
	string wname1="Result_JobSchedule_smooth_"+name;
	wname1+=".csv";
	FILE *fp,*fgp,*wp,*wjp;
	if(!(fp=fopen(fname.c_str(),"rb"))){
		printf("Open read file failure!\n");
	}
	if(!(wp=fopen(wname.c_str(),"wb"))){
		printf("Open write file failure!\n");
	}
	if(!(wjp=fopen(wname1.c_str(),"wb"))){
		printf("Open write file failure!\n");
	}

	char buf[300];
	char wbuf[100];
	fgets(buf,300,fp);
	fputs("Time(Minute),",wp);
	fputs("GreenPower,",wp);
	fputs("GridPower,",wp);
	fputs("\n",wp);

	fputs("JobNumber,",wjp);
	fputs("StartTime,",wjp);
	fputs("\n",wjp);

	char *p;
	float perPower=0;
	int subTime,rTime,deadline;
	int jobNum;

	while(!feof(fp))
	{
		memset(buf,0,300*sizeof(char));
		fgets(buf,300,fp);
		if (strcmp(buf,"")==0)
		{
			break;
		}

		if(p=strtok(buf,","))
		{
			jobNum=atoi(p);
		}
		if(p=strtok(NULL,",")){
			subTime=currentTime+atoi(p);	
		}
		for(int j=0;j<1;j++){
			p=strtok(NULL,",");
		}
		if(p=strtok(NULL,",")){
			rTime=atoi(p);
		}
		if(p=strtok(NULL,",")){
			deadline=atoi(p);
		}
		for(int j=0;j<2;j++){
			p=strtok(NULL,",");
		}
		if(p=strtok(NULL,",")){
			perPower=atof(p);
		}

		if (subTime+rTime > SYSTEM_END_TIME)
		{
			break;
		}


		Job job(jobNum,subTime,rTime,deadline,perPower);
		incomingJob.push_back(job);
		
	}
	fclose(fp);

	if(!(fgp=fopen(fname1.c_str(),"rb"))){
		printf("Open read file failure!\n");
	}
	fgets(buf,300,fgp);
	int gtime;
	float gpower;
	while(!feof(fgp))
	{
		memset(buf,0,300*sizeof(char));
		fgets(buf,300,fgp);
		if (strcmp(buf,"")==0)
		{
			break;
		}
		
		if(p=strtok(buf,","))
		{
			gtime=atoi(p);
		}
		if(p=strtok(NULL,",")){
			gpower=atof(p);	
		}
		
		for (int i=0;i<5;++i)
		{
			Grid gridInput(gtime+i,0);
			GreenEnergy gPowerInput(gtime+i,gpower);
			gridPower.push_back(gridInput);
			greenEnergy.push_back(gPowerInput);
		}

		/*Grid gridInput(gtime,0);
		GreenEnergy gPowerInput(gtime,gpower);
		gridPower.push_back(gridInput);
		greenEnergy.push_back(gPowerInput);*/
		if (greenEnergy.size()>=SYSTEM_END_TIME)
		{
			break;
		}
	}
	fclose(fgp);

	for (;currentTime<=SYSTEM_END_TIME&&incomingJob.size()>0;currentTime+=1)
	{

		while (incomingJob.size()>0&&incomingJob[0].submitTime==currentTime)
		{
			Job tempJob=*(incomingJob.begin());
			queueJob.push(tempJob);
			incomingJob.erase(incomingJob.begin());
			
		}
		if (queueJob.size()>0)
		{
			ScheduleJob();
		}
		

		/*if (currentTime%INTERVAL_TIME==1)
		{
			ScheduleJob();
		}*/

		
		//vector<Job>::iterator iterJob;
		//for (iterJob=incomingJob.begin();iterJob!=incomingJob.end();++iterJob)
		//{
		//	if (iterJob->submitTime<=currentTime)
		//	{
		//		queueJob.push(*iterJob);
		//		incomingJob.erase(iterJob);
		//		
		//	}else
		//	{
		//		break;
		//	}
		//}
		
	}

	for (int i=0;i<greenEnergy.size();++i)
	{
		memset(wbuf,0,100*sizeof(char));
		sprintf(wbuf,"%d",greenEnergy[i].time);
		fputs(wbuf,wp);
		fputs(",",wp);
		sprintf(wbuf,"%f",greenEnergy[i].power);
		fputs(wbuf,wp);
		fputs(",",wp);
		sprintf(wbuf,"%f",gridPower[i].power);
		fputs(wbuf,wp);
		fputs("\n",wp);
	}
	fclose(wp);

	for (int i=0;i<scheduledJob.size();++i)
	{
		memset(wbuf,0,100*sizeof(char));
		sprintf(wbuf,"%d",scheduledJob[i].job.jobNumber);
		fputs(wbuf,wjp);
		fputs(",",wjp);
		sprintf(wbuf,"%d",scheduledJob[i].startTime);
		fputs(wbuf,wjp);
		fputs("\n",wjp);
	}
	fclose(wjp);


	return 0;
}
