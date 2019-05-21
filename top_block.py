#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Top Block
# Generated: Fri May  3 01:25:31 2019
##################################################

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print "Warning: failed to XInitThreads()"

from PyQt4 import Qt
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import IEEE_802_15_4_CustomBlocks
import sys
from gnuradio import qtgui


class top_block(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Top Block")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Top Block")
        qtgui.util.check_set_qss()
        try:
            self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except:
            pass
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("GNU Radio", "top_block")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())


        ##################################################
        # Variables
        ##################################################
        self.samp_rate = samp_rate = 4000000
        self.center_freq_2 = center_freq_2 = 2480000000
        self.center_freq_1 = center_freq_1 = 915000000

        ##################################################
        # Blocks
        ##################################################
        self.blocks_socket_pdu_0_0 = blocks.socket_pdu("UDP_CLIENT", 'localhost', '52002', 10000, False)
        self.blocks_socket_pdu_0 = blocks.socket_pdu("UDP_SERVER", 'localhost', '52001', 10000, False)
        self.blocks_message_debug_0 = blocks.message_debug()
        self.IEEE_802_15_4_CustomBlocks_MAC_Sender_0 = IEEE_802_15_4_CustomBlocks.MAC_Sender(0x8841, 0, 0x1aaa, 0x5566, 0x1aaa, 0x6677, 0x1aaa, 0x3344)
        self.IEEE_802_15_4_CustomBlocks_MAC_Receiver_0 = IEEE_802_15_4_CustomBlocks.MAC_Receiver(0x8841, 0, 0x1aaa, 0xffff, 0x1aaa, 0x5566, 0x1aaa, 0x3344)
        self.IEEE_802_15_4_CustomBlocks_MAC_Coordinator_0 = IEEE_802_15_4_CustomBlocks.MAC_Coordinator(128, 128, 7, 0x8841, 0, 0x1aaa, 0x1aaa, 0x3344)
        self.IEEE_802_15_4_CustomBlocks_Beacon_0 = IEEE_802_15_4_CustomBlocks.Beacon(128, -1)



        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.IEEE_802_15_4_CustomBlocks_Beacon_0, 'signal out'), (self.IEEE_802_15_4_CustomBlocks_MAC_Coordinator_0, 'beacon signal in'))
        self.msg_connect((self.IEEE_802_15_4_CustomBlocks_MAC_Coordinator_0, 'mac out'), (self.IEEE_802_15_4_CustomBlocks_MAC_Sender_0, 'signal in'))
        self.msg_connect((self.IEEE_802_15_4_CustomBlocks_MAC_Receiver_0, 'out'), (self.blocks_message_debug_0, 'print'))
        self.msg_connect((self.IEEE_802_15_4_CustomBlocks_MAC_Receiver_0, 'out'), (self.blocks_socket_pdu_0_0, 'pdus'))
        self.msg_connect((self.IEEE_802_15_4_CustomBlocks_MAC_Sender_0, 'mac out'), (self.IEEE_802_15_4_CustomBlocks_MAC_Coordinator_0, 'request in'))
        self.msg_connect((self.IEEE_802_15_4_CustomBlocks_MAC_Sender_0, 'mac out'), (self.IEEE_802_15_4_CustomBlocks_MAC_Receiver_0, 'in'))
        self.msg_connect((self.blocks_socket_pdu_0, 'pdus'), (self.IEEE_802_15_4_CustomBlocks_MAC_Sender_0, 'message in'))

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "top_block")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate

    def get_center_freq_2(self):
        return self.center_freq_2

    def set_center_freq_2(self, center_freq_2):
        self.center_freq_2 = center_freq_2

    def get_center_freq_1(self):
        return self.center_freq_1

    def set_center_freq_1(self, center_freq_1):
        self.center_freq_1 = center_freq_1


def main(top_block_cls=top_block, options=None):

    from distutils.version import StrictVersion
    if StrictVersion(Qt.qVersion()) >= StrictVersion("4.5.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls()
    tb.start()
    tb.show()

    def quitting():
        tb.stop()
        tb.wait()
    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    qapp.exec_()


if __name__ == '__main__':
    main()
