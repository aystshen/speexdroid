package com.speex;

public class EchoCanceller {

    static {
        System.loadLibrary("speexdroid");
    }

    public native void open(int sampleRate, int frameSize, int filterLength);

    public native short[] process(short[] recFrame, short[] refFrame);

    public native void close();
}