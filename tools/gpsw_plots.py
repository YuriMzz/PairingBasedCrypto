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

colors=dict()

colors["05"]=['coral','violet','turquoise']
colors["10"]= ['coral','violet','turquoise']
colors["20"]=['coral','violet','turquoise']
colors["40"]=['coral','violet','turquoise']

hatches=dict()
hatches["05"]='*'
hatches["10"]='o'
hatches["20"]='.'
#colors["05"]=['red','orange','yellow']
#colors["10"]= ['green', 'chartreuse', 'yellowgreen']
#colors["20"]=['purple', 'magenta', 'orchid']

q_low=.01
q_hi=1-q_low
s_dist=2.5
ci=.95
energy_max=1000000

width=.13
size=sizes["P158"]


font = {'size'   : 25}

matplotlib.rc('font', **font)
value="gpsw_"

def main():
    data = dict()
    labels = []
    folder=sys.argv[1]
    mesure=sys.argv[2]
    alg=sys.argv[3]
    shift=0
    plt.figure(1,figsize=(25,10))
    plt.tight_layout()
    plt.ylabel('KiloBytes')
    x_ax=np.arange(1,5)
    labels_p=["gpsw_setup","gpsw_enc","gpsw_keygen", "gpsw_dec"]
    outparam=dict()
    outp=outparam["05"]=[]
    outp.append(size["gt"]+size["bn"]+(size["bn"]+size["g1"])*5) # setup output size
    outp.append(size["gt"]+size["g1"]*5)
    outp.append(size["int"]+size["g2"]*5) # TODO add policy size
    outp.append(size["gt"])
    plt.bar( x=x_ax,width = width, height=outp)
    outp=outparam["10"]=[]
    outp.append(size["gt"]+size["bn"]+(size["bn"]+size["g1"])*10)
    outp.append(size["gt"]+size["g1"]*10)
    outp.append(size["int"]+size["g2"]*10) # TODO add policy size
    outp.append(size["gt"])
    plt.bar( x=x_ax+width,width = width, height=outp)
    outp=outparam["20"]=[]
    outp.append(size["gt"]+size["bn"]+(size["bn"]+size["g1"])*20)
    outp.append(size["gt"]+size["g1"]*20)
    outp.append(size["int"]+size["g2"]*20) # TODO add policy size
    outp.append(size["gt"])
    plt.bar( x=x_ax+2*width,width = width, height=outp)
    outp=outparam["40"]=[]
    outp.append(size["gt"]+size["bn"]+(size["bn"]+size["g1"])*40)
    outp.append(size["gt"]+size["g1"]*40)
    outp.append(size["int"]+size["g2"]*40) # TODO add policy size
    outp.append(size["gt"])
    plt.bar( x=x_ax+3*width,width = width, height=outp)
    t=plt.gca().get_yticks()/1000
    plt.gca().set_yticklabels(t.astype(int))
    plt.xticks(x_ax+width*1.5,labels_p)
    plt.title("OUTPUT SIZE")
    plt.savefig(folder+'/plots/'+'m_outparams.png', bbox_inches='tight')
    
    plt.figure(2, figsize=(25,10))
    plt.ylabel('KiloBytes')
    inparam=dict()
    inp=inparam["05"]=[]
    inp.append(size["int"])
    inp.append(2*size["gt"]+size["int"]+size["g1"]*5)
    inp.append(size["bn"]+size["bn"]*5+size["int"]+size["char"]*5*2) # TODO specify policy string size
    inp.append(outparam["05"][2]+outparam["05"][1]+size["int"]) # cph + key
    plt.bar( x=x_ax,width = width, height=inp , label="05")
    inp=inparam["10"]=[]
    inp.append(size["int"])
    inp.append(2*size["gt"]+size["int"]+size["g1"]*10)
    inp.append(size["bn"]+size["bn"]*10+size["int"]+size["char"]*10*2) # TODO specify policy string size
    inp.append(outparam["10"][2]+outparam["10"][1]+size["int"]) # cph + key
    plt.bar( x=x_ax+width,width = width, height=inp, label="10")
    inp=inparam["20"]=[]
    inp.append(size["int"])
    inp.append(2*size["gt"]+size["int"]+size["g1"]*20)
    inp.append(size["bn"]+size["bn"]*20+size["int"]+size["char"]*20*2) # TODO specify policy string size
    inp.append(outparam["20"][2]+outparam["20"][1]+size["int"]) # cph + key
    plt.bar( x=x_ax+2*width,width = width, height=inp, label="20")
    inp=inparam["40"]=[]
    inp.append(size["int"])
    inp.append(2*size["gt"]+size["int"]+size["g1"]*40)
    inp.append(size["bn"]+size["bn"]*40+size["int"]+size["char"]*40*2) # TODO specify policy string size
    inp.append(outparam["40"][2]+outparam["40"][1]+size["int"]) # cph + key
    plt.bar( x=x_ax+3*width,width = width, height=inp  , label="40")
    plt.xticks(x_ax+width*1.5,labels_p)
    t=plt.gca().get_yticks()/1000
    plt.gca().set_yticklabels(t.astype(int))
    plt.xticks(x_ax+width*1.5,labels_p)
    plt.legend(title="number of attributes",loc='upper left')
    plt.title("INPUT SIZE")
    plt.savefig(folder+'/plots/'+'m_inparams.png', bbox_inches='tight')






    plt.figure(3,figsize=(25,10))
    files=os.listdir(folder)
    print(files)
    files=[x for x in files if mesure in x]
    files=[x for x in files if alg in x]
    for b_file in sorted(files, key=lambda x: int(x[-len(mesure)-7 : -len(mesure)-5])):
        attr = b_file[12:14]
        print(attr)
        df = pd.read_csv(folder+"/"+b_file) 
        #if(mesure=="SPACE" and len(sys.argv)>3):
            #df=df[df['function'].str.contains("res_size")]
        #if(mesure=="SPACE" and len(sys.argv)==3):
        #    df=df[df['function'].str.contains("res_size")==False]
        #df=df[df["function"]=="gpsw_enc"]
        df=df[df['function'].str.contains(value)]
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
            #print(stdev)
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
        print(data)
    #print(data["B24-P315"])
    #print(labels)
        x_ax=np.arange(0,len(labels))+1  
        try:
            #colors=['red', 'green', 'blue', 'cyan']
            if(mesure=="SPACE1"):
                inparams=inparam[attr][1]
                fun=data[0]+inparams
                #print(inparam[attr])
                outparams=fun+outparam[attr][1]
                plt.bar(x=x_ax+shift, width = width, height=outparams, 
                yerr=data[1], color=colors[attr][0], label=attr+"outparam",
                #hatch=hatches[attr]
                )
                plt.bar(x=x_ax+shift, width = width, height=fun,
                color=colors[attr][2], label=attr, 
                #hatch=hatches[attr]
                )
                plt.bar(x=x_ax+shift, width = width, height=inparams, 
                color=colors[attr][1], label=attr+"inparam", 
                #hatch=hatches[attr]
                )
            else:
                plt.bar(x=x_ax+shift, width = width, height=data[0]/1000, 
                yerr=data[1]/1000,  label=attr,
                )
            
            shift+=(width)
        except KeyError as e:
            #print(attr+" "+mesure+"   "+str(e))
            pass
    try: 
        os.mkdir(folder+'/plots')
    except(FileExistsError):
        pass
    title=mesure+" for 78 bits of security "+alg+"     C.I."+str(ci*100)+"%"
    plt.title(mesure)
    if(mesure=='TIME'):
        plt.ylabel("seconds")
    elif(mesure=='ENERGY'):
        plt.ylabel('Wh')
    else:
        plt.ylabel('KiloBytes')
    if(mesure!='SPACE'):
        plt.legend(title="number of attributes",loc='upper left' )
    plt.xticks(x_ax+((shift-width)/2),labels)
    plt.savefig(folder+'/plots/'+'m_'+mesure+'.png', bbox_inches='tight')
    plt.show()
   
    

if __name__ == '__main__':
	exit(main())