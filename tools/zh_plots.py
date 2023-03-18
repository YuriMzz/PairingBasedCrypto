#!/usr/bin/python3
import os
import sys
import pandas as pd
import numpy as np
import scipy.stats as st
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import matplotlib
from operator import getitem
from size import size as sizes
import locale


q_low=.01
q_hi=1-q_low
s_dist=2.1
ci=.95
energy_max=1000000

width=.16
font = {'size'   : 25}

matplotlib.rc('font', **font)
def main():
    folder=sys.argv[1]
    plt.figure(1, figsize=(25,10))
    plt.ylabel('KiloBytes')
    x_ax=np.arange(1,5)
    labels=["zh_setup","zh_enc","zh_keygen", "zh_dec"]
    size=sizes["P158"]
    pubsize_c=size["size_t"]+size["g1"]
    pubsize_v=size["g2"]*3+size["g1"]*6
    mksize=size["bn"]*2
    cphsize=size["size_t"]+size["g2"]+size["g1"]
    prvsize_c=size["size_t"]+size["g1"]
    prvsize_v=size["g1"]*2
    outparam=dict()
    outp=outparam["05"]=[]
    outp.append(pubsize_c+pubsize_v*5+mksize) # setup output size
    outp.append(cphsize) # TODO add policy size
    outp.append(prvsize_c+prvsize_v*5) 
    outp.append(size["gt"])
    plt.bar( x=x_ax,width = width, height=outp)
    outp=outparam["10"]=[]
    outp.append(pubsize_c+pubsize_v*10+mksize) # setup output size
    outp.append(cphsize) # TODO add policy size
    outp.append(prvsize_c+prvsize_v*10) 
    outp.append(size["gt"])
    plt.bar( x=x_ax+width,width = width, height=outp)
    outp=outparam["20"]=[]
    outp.append(pubsize_c+pubsize_v*20+mksize) # setup output size
    outp.append(cphsize) # TODO add policy size
    outp.append(prvsize_c+prvsize_v*20) 
    outp.append(size["gt"])
    plt.bar( x=x_ax+2*width, width = width, height=outp)
    t=plt.gca().get_yticks()/1000
    plt.gca().set_yticklabels(t.astype(int))
    plt.xticks(x_ax+width,labels)
    plt.title("OUTPUT SIZE")
    plt.savefig(folder+'/plots/'+'m_outparams.png', bbox_inches='tight')
    plt.figure(2, figsize=(25,10))

    inparam=dict()
    inp=inparam["05"]=[]
    inp.append(0)
    inp.append(size["gt"]+pubsize_c+pubsize_v*5) # TODO specify policy string size
    inp.append(outparam["05"][0]) # TODO specify policy string size
    inp.append(outparam["05"][2]+outparam["05"][1]+pubsize_c+pubsize_v*5) # cph + key
    plt.bar( x=x_ax,width = width, height=inp, label="05")
    inp=inparam["10"]=[]
    inp.append(0)
    inp.append(size["gt"]+pubsize_c+pubsize_v*10) # TODO specify policy string size
    inp.append(outparam["10"][0]) # TODO specify policy string size
    inp.append(outparam["10"][2]+outparam["10"][1]+pubsize_c+pubsize_v*10) # cph + key
    plt.bar( x=x_ax+width,width = width, height=inp, label="10")
    inp=inparam["20"]=[]
    inp.append(0)
    inp.append(size["gt"]+pubsize_c+pubsize_v*20) # TODO specify policy string size
    inp.append(outparam["20"][0]) # TODO specify policy string size
    inp.append(outparam["20"][2]+outparam["20"][1]+pubsize_c+pubsize_v*20) # cph + key
    plt.bar( x=x_ax+2*width, width = width, height=inp, label="20")
    plt.xticks(x_ax+width,labels)
    t=plt.gca().get_yticks()/1000
    plt.gca().set_yticklabels(t.astype(int))
    plt.title("INPUT SIZE")
    plt.legend(title="number of attributes",loc='upper left' )
    plt.ylabel('KiloBytes')
    plt.savefig(folder+'/plots/'+'m_inparams.png', bbox_inches='tight')
    #plt.show()
    #exit()













    data = dict()
    labels = []
    folder=sys.argv[1]
    mesure=sys.argv[2]
    shift=0
    plt.figure(figsize=(25,10))
    files=os.listdir(folder)
    print(files)
    files=[x for x in files if mesure in x]
    print(files)
    for b_file in sorted(files, key=lambda x: int(x[-len(mesure)-7 : -len(mesure)-5])):
        attr = b_file[8:10]
        print(attr)
        df = pd.read_csv(folder+"/"+b_file) 
        #if(mesure=="SPACE" and len(sys.argv)>3):
            #df=df[df['function'].str.contains("res_size")]
        #if(mesure=="SPACE" and len(sys.argv)==3):
        #    df=df[df['function'].str.contains("res_size")==False]
        #df=df[df["function"]=="gpsw_enc"]
        if len(labels)==0:
            labels= df["function"].drop_duplicates().values.tolist()
            print(labels)
            
        #
        #print(df)
        if(mesure=="ENERGY" and df["function"].duplicated().any()):
            #filter=(df.metric.values<energy_max)
            #df["metric"]=df["metric"].apply(lambda x: x*21000)
            res= df.groupby("function")["metric"].quantile([q_low, q_hi]).unstack(level=1)
            stdev= df.groupby(by=["function"], sort=False).std()
            stdev=stdev.apply(lambda x: np.nan_to_num(x))
            print(stdev)
            
            mean = df.groupby(by=["function"], sort=False).mean()
            #q_filter=((res.loc[df.function, q_low] < df.metric.values) & (df.metric.values < res.loc[df.function, q_hi]))
            s_filter=(
                stdev.loc[df.function, 'metric']*s_dist >= abs(df["metric"].values- mean.loc[df.function, 'metric']))    
            #print(stdev)    
            #a_filter=(df.function.values=="gpsw_dec").flatten()
            inv_filter=(
                stdev.loc[df.function, 'metric']*s_dist <= abs(df["metric"].values- mean.loc[df.function, 'metric']))   
            #print(df.loc[(inv_filter.values )])
            df=df.loc[(s_filter ).values]

        df["function"]=df["function"].apply(lambda x:labels.index(x))
        print(df.groupby(by=["function"], sort=True).mean())    
        values= df.groupby(by=["function"], sort=True).mean().values.flatten()
        #print(df.groupby(by=["function"], sort=False).max().values.flatten()-df.groupby(by=["function"], sort=False).min().values.flatten())
        stdev= df.groupby(by=["function"], sort=True).std().values.flatten()
        print(df.groupby(by=["function"], sort=True).std())  
        
        n_samples= df.groupby(by=["function"], sort=True).count().values.flatten()
        print(n_samples)
        errors =st.norm.ppf(ci) * stdev / np.sqrt(n_samples)
        data=[]
        data.append(values)
        data.append(errors)
        #print(data)
    #print(data["B24-P315"])
    #print(labels)
    
        x_ax=np.arange(0,len(labels))+1  
        try:
            if(mesure=='SPACE1'):
                tot=data[0]+outparam[attr]+inparam[attr]
                params=tot-data[0]
                plt.bar(x=x_ax+shift, width = width, height=tot, yerr=data[1], label=attr)
                plt.bar(x=x_ax+shift, width = width, height=params,  label=attr+"inparam", 
                #hatch="*"
                )
                plt.bar(x=x_ax+shift, width = width, height=outparam[attr],  label=attr+"outparam",
                #hatch="o"
                )
            else:
                plt.bar(x=x_ax+shift, width = width, height=data[0]/1000, yerr=data[1]/1000, label=attr)
            shift+=width
        except KeyError as e:
            #print(attr+" "+mesure+"   "+str(e))
            pass
    try: 
        os.mkdir(folder+'/plots')
    except(FileExistsError):
        pass
    #title=mesure+"stack + in + out  C.I."+str(ci*100)+"%"
    title=mesure
    plt.title(title)
    if(mesure=='TIME'):
        plt.ylabel("seconds")
        plt.legend(title="number of attributes",loc='upper right' )
    elif(mesure=='ENERGY'):
        plt.ylabel('Wh')
    else:
        plt.ylabel('KiloBytes')
   
    plt.xticks(x_ax+((shift-width)/2),labels)
    """ if(mesure=="ENERGY" and alg=="COM"):
        plt.ylim(top=3)
    if(mesure=="TIME" and alg=="COM"):
        plt.ylim(top=75000) """
    plt.savefig(folder+'/plots/'+'m_'+title+'.png', bbox_inches='tight')
    plt.show()
   
    

if __name__ == '__main__':
	exit(main())