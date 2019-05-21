/* -*- c++ -*- */

#define IEEE_802_15_4_CUSTOMBLOCKS_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "IEEE_802_15_4_CustomBlocks_swig_doc.i"

%{
#include "IEEE_802_15_4_CustomBlocks/MAC_Tx.h"
#include "IEEE_802_15_4_CustomBlocks/receiver_packet_sink.h"
#include "IEEE_802_15_4_CustomBlocks/MAC_Rx.h"
#include "IEEE_802_15_4_CustomBlocks/Beacon.h"
#include "IEEE_802_15_4_CustomBlocks/MAC_Coordinator.h"
#include "IEEE_802_15_4_CustomBlocks/MAC_Sender.h"
#include "IEEE_802_15_4_CustomBlocks/AES.h"
#include "IEEE_802_15_4_CustomBlocks/MAC_Receiver.h"
%}

%include "IEEE_802_15_4_CustomBlocks/MAC_Tx.h"
GR_SWIG_BLOCK_MAGIC2(IEEE_802_15_4_CustomBlocks, MAC_Tx);

%include "IEEE_802_15_4_CustomBlocks/receiver_packet_sink.h"
GR_SWIG_BLOCK_MAGIC2(IEEE_802_15_4_CustomBlocks, receiver_packet_sink);
%include "IEEE_802_15_4_CustomBlocks/MAC_Rx.h"
GR_SWIG_BLOCK_MAGIC2(IEEE_802_15_4_CustomBlocks, MAC_Rx);

%include "IEEE_802_15_4_CustomBlocks/Beacon.h"
GR_SWIG_BLOCK_MAGIC2(IEEE_802_15_4_CustomBlocks, Beacon);

%include "IEEE_802_15_4_CustomBlocks/MAC_Coordinator.h"
GR_SWIG_BLOCK_MAGIC2(IEEE_802_15_4_CustomBlocks, MAC_Coordinator);
%include "IEEE_802_15_4_CustomBlocks/MAC_Sender.h"
GR_SWIG_BLOCK_MAGIC2(IEEE_802_15_4_CustomBlocks, MAC_Sender);
%include "IEEE_802_15_4_CustomBlocks/AES.h"
GR_SWIG_BLOCK_MAGIC2(IEEE_802_15_4_CustomBlocks, AES);
%include "IEEE_802_15_4_CustomBlocks/MAC_Receiver.h"
GR_SWIG_BLOCK_MAGIC2(IEEE_802_15_4_CustomBlocks, MAC_Receiver);
