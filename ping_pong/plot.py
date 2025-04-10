import matplotlib.pyplot as plt
import numpy as np
from scipy.optimize import curve_fit


def line(x, m, c):
    return m * x + c

def plotFile(fig, ax, filename, markerColor, lineColor, dlabel):

    # getting file data, message size data, communication delay time
    fdata = np.loadtxt(filename, dtype=str, delimiter=",")
    commtime = np.array(fdata[1:,3], dtype=float)
    msize = np.array(fdata[1:,0], dtype=int) * 4 * 1e-6          # converting ints to Mb    

    # plotting data and line of best fit
    ax.scatter(msize, commtime, label=f"Raw Data: {dlabel}", color=markerColor)
    popt, pcov = curve_fit(line, msize, commtime)

    xrange = np.linspace(0, 6.5, 100)
    ax.plot(xrange, line(xrange, *popt), linestyle="--", color=lineColor, label=f"Best Fit: {dlabel}")

    # formatting axis names
    ax.set_title("Comm Delay vs Message Size")
    ax.set_xlabel("Message Size [Mb]")
    ax.set_ylabel("Avg. Comm Time [s]")
    ax.legend()    

    # formatting axis size limits
    # ax.set_xlim([0.0, 6.9])
    # ax.set_ylim([0.0, 0.00215])

    m = popt[0]
    bandwidth = 1/m

    c = popt[1]

    print(bandwidth)
    print(c)

    # fig.text(0.6, 0.5, f'latency: {np.round(c, 7)}s')
    # fig.text(0.6, 0.46, f'bandwidth: {np.round(bandwidth, 2)}Mb/s')

    return 0

def main():



    fig, ax = plt.subplots(figsize=(8,6))
    plt.subplots_adjust(left=0.13, right=0.96, top=0.93, bottom=0.09)

    markerColor = ["tab:red", "tab:blue", "tab:green"]
    # lineColor = ["r", "b", "g"]

    plotFile(fig, ax, "SsendData.txt", markerColor[0], markerColor[0], "Ssend")
    plotFile(fig, ax, "SendData.txt", markerColor[1], markerColor[1], "Send")

    fig.savefig('pingpong.png', format='png')

    # --------------------------------------------------------------------------------------------------------------

    # The idea is to calculate the relationship between latency and bandwidth is 
    # delay = bandwidth / packetsize + latency  ->  i.e. y = m*x + c  

    # delay is the communication time
    # packetsize is the message size
    # bandwidth is the gradient
    # latency is the y-intercept

    return 0

main()