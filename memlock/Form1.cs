using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace memlock
{
    public partial class MemlockForm : Form
    {
        static internal string sizeToString(long size)
        {
            if (size < 1024)
            {
                return String.Format("{0}MB", size);
            }
            else
            {
                return String.Format("{0:F2}GB", size / 1024.0);
            }
        }

        [DllImport("kernel32.dll")]
        static extern uint FormatMessage(
              uint dwFlags, IntPtr lpSource,
              uint dwMessageId, uint dwLanguageId,
              StringBuilder lpBuffer, int nSize,
              IntPtr Arguments);
        private const uint FORMAT_MESSAGE_FROM_SYSTEM = 0x00001000;

        class MemInfo
        {
            public long size;
            public IntPtr ptr;
            public override string ToString()  
            {
                return "0x" + ptr.ToInt64().ToString("x16") + "("+size.ToString() + "MB)";
            }
        };
        class MemInfos
        {
            public List<MemInfo> memInfos=new List<MemInfo>();
            private long totalSize()
            {
                long size = 0;
                foreach (var info in memInfos)
                {
                    size+=info.size;
                }
                return size;
            }
            public override string ToString()
            {
                return string.Format("{0} (divide:{1}", sizeToString(totalSize()), memInfos.Count);
            }
        }


        List<MemInfos> memList = new List<MemInfos>();
        Allocator allocator=new Allocator();

        public MemlockForm()
        {
            InitializeComponent();
            if(!allocator.initilalize(System.Diagnostics.Process.GetCurrentProcess()))
            {
                MessageBox.Show("initialize failed.");
                Close();
                return;
            }
            var gpus = allocator.getGpuInfomations();
            foreach (var info in gpus)
            {
                cboMemDevice.Items.Add(info.name);
            }
        }

        private void MemlockForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            allocator.finalize();
        }


        private void freeMemorys(MemInfos memInfos)
        {
            foreach(var memInfo in memInfos.memInfos)
            {
                allocator.free(memInfo.ptr, new IntPtr(memInfo.size));
            }
        }

        private void btnAlloc_Click(object sender, EventArgs e)
        {
            long memSize = (long)(numAllocSize.Value);
            long allocCount = (long)(numAllocCount.Value);
            memSize = (memSize + 3) / 4 * 4;
            var infos = new MemInfos();
            for (int i = 0; i < allocCount; i++)
            {
                var device = cboMemDevice.SelectedIndex;
                IntPtr ptr= IntPtr.Zero;
                if (device == 0)
                {
                    ptr = allocator.alloc(new IntPtr(memSize * 1024 * 1024));
                }
                else
                {
                    ptr = allocator.gpuAlloc(device-1, new IntPtr(memSize * 1024 * 1024));
                }
                if (ptr == IntPtr.Zero)
                    break;

                var info = new MemInfo();
                info.ptr = ptr;
                info.size = memSize;
                infos.memInfos.Add(info);
            }

            if(infos.memInfos.Count==0)
            {
                checkWin32Error(System.Runtime.InteropServices.Marshal.GetLastWin32Error());
                MessageBox.Show("failed.");
            }
            else
            {
                memList.Add(infos);
                refreshList();
            }
        }

        private void btnFree_Click(object sender, EventArgs e)
        {
            var idx = lstAllocList.SelectedIndex;
            if (idx >= 0)
            {
                var infos = memList[idx];
                freeMemorys(infos);
                memList.Remove(infos);
            }
            refreshList();
        }

        private void refreshList()
        {
            var selected = lstAllocList.SelectedIndex;
            lstAllocList.Enabled = false;
            lstAllocList.Items.Clear();
            foreach (var item in memList)
            {
                lstAllocList.Items.Add(item);
            }
            if (lstAllocList.Items.Count > selected) 
            {
                lstAllocList.SelectedIndex = selected;
            }
            lstAllocList.Enabled = true;
        }

        void checkWin32Error(int err)
        {
            StringBuilder message = new StringBuilder(255);

            FormatMessage(
              FORMAT_MESSAGE_FROM_SYSTEM,
              IntPtr.Zero,
              (uint)err,
              0,
              message,
              message.Capacity,
              IntPtr.Zero);

            Console.WriteLine("Win32LastError：" +
              message.ToString());
        }

        private void refreshTotalSize()
        {
            long size = (long)numAllocSize.Value * (long)numAllocCount.Value;
            lblTotalSize.Text = sizeToString(size);
        }

        private void numAllocSize_ValueChanged(object sender, EventArgs e)
        {
            refreshTotalSize();
        }

        private void numAllocCount_ValueChanged(object sender, EventArgs e)
        {
            refreshTotalSize();
        }

    }
}
