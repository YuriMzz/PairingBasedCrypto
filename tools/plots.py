#!/usr/bin/python3
import os
import sys
import pandas as pd
import numpy as np
import scipy.stats as st
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import matplotlib
import plotinpy as pnp


sec_levels=dict()
sec_levels["128"] = ["B12-P446", "BN-P446", "B12-P383", "B12-P381",  "B12-P377" ,"B24-P315"]
sec_levels["112"] = ["BN-P256", "BN-P254"]
sec_levels["80"] = ["BN-P158"]

curves=sec_levels["128"]+sec_levels["112"]+sec_levels["80"]
#curves.reverse()

q_low=.01
q_hi=1-q_low
s_dist=2.7
ci=.99
energy_max=1000000

value="zss"
def main():
    data = dict()
    labels = []
    folder=sys.argv[1]
    mesure=sys.argv[2]
   
    for b_file in os.listdir(folder):
        if mesure not in b_file:
            continue
        curve = b_file[:-(len(mesure)+5)]
        df = pd.read_csv(folder+"/"+b_file) 
        if(mesure=="SPACE" and len(sys.argv)>3):
            df=df[df['function'].str.contains("res_size")]
        if(mesure=="SPACE" and len(sys.argv)==3):
            df=df[df['function'].str.contains("res_size")==False]
        df=df[df['function'].str.contains(value)]
        #df=df[~df['function'].str.contains("ibe")]
        df=df[~df['function'].str.contains("cls")]

        if len(labels)==0:
            labels= df["function"].drop_duplicates().values.tolist()
       

        if(mesure=="ENERGY"):
            #filter=(df.metric.values<energy_max)
            #df=df.loc[filter]
            res= df.groupby("function")["metric"].quantile([q_low, q_hi]).unstack(level=1)
            stdev= df.groupby(by=["function"], sort=False).std()
            mean = df.groupby(by=["function"], sort=False).mean()
            #q_filter=((res.loc[df.function, q_low] < df.metric.values) & (df.metric.values < res.loc[df.function, q_hi]))
            s_filter=(stdev.loc[df.function, 'metric']*s_dist > abs(df.metric.values- mean.loc[df.function, 'metric']))        
            #print(s_filter)
            df=df.loc[s_filter.values]
        df["function"]=df["function"].apply(lambda x:labels.index(x))
        values= df.groupby(by=["function"], sort=False).mean().values.flatten()
        #print(df.groupby(by=["function"], sort=False).max().values.flatten()-df.groupby(by=["function"], sort=False).min().values.flatten())
        stdev= df.groupby(by=["function"], sort=False).std().values.flatten()
        n_samples= df.groupby(by=["function"], sort=False).count().values.flatten()
        errors =st.norm.ppf(ci) * stdev / np.sqrt(n_samples)
        for sec in sec_levels.keys():
            if(curve in sec_levels[sec]):
                data[curve]=[]
                data[curve].append(values)
                data[curve].append(errors)
    #print(data["B24-P315"])
    #print(labels)
    font = {'size'   : 45}
    if(mesure=="SPACE" and len(sys.argv)>3):
        mesure="SIZE"
        font = {'size'   : 50}
    elif(len(labels)==1):
        font = {'size'   : 25}
    try: 
        os.mkdir(folder+'/plots')
    except(FileExistsError):
        pass
    shift=0
    width=.09
    fig=0
   

    matplotlib.rc('font', **font)
    x_ax=np.arange(0,len(labels))+1
    for sec in sec_levels.keys():
        if(mesure=="SIZE"):
            plt.figure(fig,figsize=(40,14))
        else:
            if len(labels)>1:
                plt.figure(fig,figsize=(35,10))
            else:
                plt.figure(fig,figsize=(20,10))
        plt.tight_layout()
        shift=0
        title=mesure+" "+value
        plt.title(mesure)
        for curve in curves:
            h=data[curve][0]
            ye=data[curve][1]
            if(mesure!="SIZE"):
                h=h/1000
                ye=ye/1000
            
            #plt.yticks(np.arange(0,11,0.1))
            try:
                plt.bar(x=x_ax+shift, width = width, height=h, yerr=ye, label=curve)
                if(labels[0]=="zss_res_size"):
                    plt.ylim(top=150)
                if(mesure=="SIZE"):
                    plt.text(x=x_ax+shift, horizontalalignment="center", y=5, s=str(int(h[0])))
                shift+=width
            except KeyError as e:
                #print(curve+" "+mesure+"   "+str(e))
                continue
        #plt.bar(x=x_ax+(width*5), width = width, height=7, bottom=135, color="white")
        #plt.text(x=x_ax+(width*5), y=131, s="/", horizontalalignment="center", rotation=-50)
        #plt.text(x=x_ax+(width*5), y=138, s="/", horizontalalignment="center", rotation=-50)
        #plt.text(x=x_ax+(width*5), y=131, s="~", horizontalalignment="center", rotation=0)
        if(mesure=="SIZE"):
            plt.bar(x=x_ax-width*2.5, width = width, height=64, label="ECDSA-secp256r1", color="white", edgecolor='violet', hatch="*")
            plt.text(x=x_ax-width*2.5, horizontalalignment="center", y=5, s=str(64))
            plt.bar(x=x_ax-width*1.5, width = width, height=40, label="ECDSA-secp160r1", color="white", edgecolor='yellowgreen', hatch="*")
            plt.text(x=x_ax-width*1.5, horizontalalignment="center", y=5, s=str(40))
        if(labels[0]=="zss_res_size"):
            plt.text(x=x_ax+(width*5), y=140, s="~", horizontalalignment="center", fontsize=150,rotation=0)
            
        #plt.bar(x=x_ax+shift, width = width, height=500, bottom=300)
        #plt.legend(title="Curves")
        
        #plt.xticks(x_ax)
        
        if(mesure=='TIME'):
            plt.ylabel("seconds")
            ylabels=[str(int(x//1000))+"s" if (x>=1000 or x==0)
                    else 
                    str(x//1000)+"s"  
                    for x in plt.gca().get_yticks()]
            #ylabels=[str(x)+"s" for x in plt.gca().get_yticklabels() ]
            if(len(labels)!=1):
                plt.legend(title="Curves", loc='best', bbox_to_anchor=(1, 1.15, 0., 0.))
            else:
                plt.legend(title="Curves", loc='best')
        elif(mesure=='ENERGY'):
            plt.ylabel('Wh')
            ylabels=[str(x)+"Wh" for x in plt.gca().get_yticks()]
        elif(mesure=='SPACE'):
            plt.ylabel('KiloBytes')
            ylabels=[str(int(x)//1000)+"kB" for x in plt.gca().get_yticks()]
            if(len(labels)!=1):
                plt.legend(title="Curves", loc='best', bbox_to_anchor=(1, 1.15, 0., 0.))
        else:
            ylabels=[str(int(x))+"B" for x in plt.gca().get_yticks()]
            #ylabels=plt.gca().get_yticks()
            #ylabels=[]
            plt.ylabel('bytes')
            plt.legend(title="Curves", loc='best', bbox_to_anchor=(1, 1.1, 0., 0.))
        #plt.gca().set_yticklabels(ylabels)
        plt.xticks(x_ax+((shift-width)/2),labels)
        if(len(labels)==1):
            plt.xticks([])

        print(labels)
        #plt.gca().set_xticklabels(labels)
        #plt.tick_params(axis='y', style='plain')
        
        plt.savefig(folder+'/plots/'+folder+'_m'+'_'+title+'.png', bbox_inches='tight')
        fig+=1
        break
    plt.show()

    """ plt.figure(fig,figsize=(20,10))
    #plt.figure(fig,figsize=(15,10))
    plt.tight_layout()

    shift=0
    title=mesure+" "+value
    plt.title(mesure)
    
    x_ax=1
    for curve in curves:
        #plt.yticks(np.arange(0,11,0.1))
        try:
            plt.bar(x=x_ax+shift, width = width, height=data[curve][0], yerr=data[curve][1], label=curve)
            shift+=width
            print(data[curve][0])
        except KeyError as e:
            #print(curve+" "+mesure+"   "+str(e))
            continue
    if(mesure=='TIME'):
        plt.ylabel("ms")
        plt.legend(title="Curves")
    elif(mesure=='ENERGY'):
        plt.ylabel('Wh')
    else:
        plt.ylabel('bytes')
    if(mesure=="SIZE"):
        plt.legend(title="Curves", loc='best', bbox_to_anchor=(1, .9, 0., 0.))
        
    plt.xticks([])
    plt.savefig(folder+'/plots/'+folder+'_'+title+'.png', bbox_inches='tight')
    plt.show() """

if __name__ == '__main__':
	exit(main())