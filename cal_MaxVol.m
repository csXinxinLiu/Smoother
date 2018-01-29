function MaxVol=cal_MaxVol()

    datain = csvread('wind_power_E101-44912-2011-05-5min.csv',1,0);
	
    power_h=reshape(datain(:,5),12,length(datain)/12);
	
    var_powerh=var(power_h,1,1);
	
    csvwrite('var_powerh.csv',var_powerh');
	
    figure;
	
    plot(var_powerh);
	
    xlabel('Hour');
	
    ylabel('Variance');
	
    title(['Variance of the power','(Month)'],'Fontsize',16);
	
    saveas(gcf,'Var_power_month','jpg');

    vmin=min(var_powerh);
	
    vmax=max(var_powerh);
	
    vx_month=linspace(vmin,vmax,100);
	
    f=ksdensity(var_powerh);
	
    figure;
	
    plot(vx_month,f,'Linewidth',2);
	
    xlabel('Varience');
	
    ylabel('Probability');

    [fd,x]=ecdf(var_powerh);
	
    figure;
	
    plot(x,fd,'Linewidth',2);
	
    xlabel('Variance');
	
    ylabel('CDF');
	
    [err,loc]=min(abs(fd-0.95));
	
    thres=x(loc);
	
    devia=zeros(1,length(var_powerh));
	
    h_reg=zeros(1,length(var_powerh));
	
    h_count=1;
	
    for ii=1:length(var_powerh)
	
        if(var_powerh(ii)<thres)
		
            max_devia=abs(max(power_h(:,ii))-mean(power_h(:,ii)));
			
            min_devia=abs(min(power_h(:,ii))-mean(power_h(:,ii)));
			
            devia(h_count)=max(max_devia,min_devia);
			
            h_reg(h_count)=ii;
			
            h_count=h_count+1;
			
        end
		
    end
    
    devia(h_count:end)=[];
	
    h_reg(h_count:end)=[];
	
    figure;
	
    plot(devia);
	
    [fdevia,xdevia]=ecdf(devia);
	
    figure;
	
    plot(xdevia,fdevia);
	
    [err,loc1]=min(abs(fdevia-0.99));
	
    MaxVol=xdevia(loc1);
    

    S=zeros(size(power_h));
	
    Ave_powerH=zeros(size(power_h));
	
    for is=1:length(var_powerh)
	
        if (any(h_reg==is))
		
            [S(:,is), errs]=findS(power_h(:,is),MaxVol);
			
        else
		
            S(:,is)=zeros(12,1);
			
        end
		
        Ave_powerH(:,is)=power_h(:,is)+S(:,is);
		
    end
	
    Ave_power=reshape(Ave_powerH,1,length(datain));
	
    csvwrite('greenPower-05-91011_X10-smooth.csv',[datain(:,1),Ave_power']);
	


    load1=csvread('cluster_datacenter_power_2011_05_five_minute.csv',1,0);
	
    load1(:,3)=load1(:,3)/1.5-300*0.05*11000;
	
    x1=zeros(3,length(load1));
	
    x10=[0;0;0];
	
    x10_ave=[0;0;0];
    
    x10_comparison=[0;0;0];
	
    SW=0;
	
    SW_ave=0;
    
    SW_comparison=0;
    
    int_Pwind=interp1((0:length(datain)-1)*5,datain(:,5),load1(:,1),'linear');
		
    int_Pwind_ave=interp1((0:length(datain)-1)*5,Ave_power,load1(:,1),'linear');
    
    %%
    NowVol=0; 

    aaa=0; % aaa = datacenter power - wind power

    Comparison_power=zeros(length(int_Pwind),1); 

    for iss=1:length(int_Pwind) 
    
        aaa=load1(iss,3)-int_Pwind(iss);  
     
        if (aaa>0) 
      
            if NowVol >= aaa  
         
                Comparison_power(iss)=int_Pwind(iss)+aaa;   
         
                NowVol=NowVol-aaa; 
       
            else
                
                Comparison_power(iss)=int_Pwind(iss)+NowVol;
         
                NowVol=0;
       
            end
            
        else  
        
            Comparison_power(iss)=int_Pwind(iss)+aaa;
        
            NowVol=min(MaxVol,NowVol-aaa);
     
        end  
        
    end   
    
    int_Pwind_comparison=Comparison_power;
    %%
	
    for i1=1:length(load1)
	
        A1=[186,186,62;186,0,0;1,1,1];
		
        b1=[load1(i1,3),int_Pwind(i1),11000]';
		
        b1_ave=[load1(i1,3),int_Pwind_ave(i1),11000]';
       
        b1_comparison=[load1(i1,3),int_Pwind_comparison(i1),11000]';
		
        x1=A1\b1;
		
        x1_ave=A1\b1_ave;
        
        x1_comparison=A1\b1_comparison;
		
        if((x1(1)-x10(1))*(x1(2)-x10(2))<0)
		
            SW=SW+min(abs(x1(1)-x10(1)),abs(x1(2)-x10(2))); % The switching times without Flexible Smoothing.
        end
		
        x10=x1;
		
        if((x1_ave(1)-x10_ave(1))*(x1_ave(2)-x10_ave(2))<0)
		
            SW_ave=SW_ave+min(abs(x1_ave(1)-x10_ave(1)),abs(x1_ave(2)-x10_ave(2))); % The switching times with Flexible Smoothing.
			
        end
		
        x10_ave=x1_ave;
        
        if((x1_comparison(1)-x10_comparison(1))*(x1_comparison(2)-x10_comparison(2))<0)
        
            SW_comparison=SW_comparison+min(abs(x1_comparison(1)-x10_comparison(1)),abs(x1_comparison(2)-x10_comparison(2)));% The switching times with the general battery storage solution.
    
        end
        
        x10_comparison=x1_comparison;
		
    end
	
    fprintf('SW=%d, SW_ave=%d, SW_comparison=%d\n',SW,SW_ave,SW_comparison);
    fprintf('MaxVol=%d\n',MaxVol);
    
    fprintf('(SW_comparison - SW_ave) /SW_comparison=%f\n',(SW_comparison - SW_ave) /SW_comparison);
    fprintf('\n');


end
