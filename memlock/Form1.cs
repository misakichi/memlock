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
        [DllImport("kernel32.dll")]
        static extern uint FormatMessage(
              uint dwFlags, IntPtr lpSource,
              uint dwMessageId, uint dwLanguageId,
              StringBuilder lpBuffer, int nSize,
              IntPtr Arguments);
        private const uint FORMAT_MESSAGE_FROM_SYSTEM = 0x00001000;

        //static uint MEM_4MB_PAGES = 0x80000000u;
        //static uint MEM_RESERVE = 0x00002000;
        //static uint MEM_COMMIT= 0x00001000;
        //static uint MEM_PHYSICAL = 0x00400000;
        //static uint MEM_RELEASE = 0x00008000;
        //static uint MEM_DECOMMIT= 0x00004000;
        //private const uint PAGE_NOACCESS = 0x1;

        struct MemInfo
        {
            public long size;
            public IntPtr ptr;
            public override string ToString()  
            {
                return "0x" + ptr.ToInt64().ToString("x16") + "("+size.ToString() + "MB)";
            }
        };
        List<MemInfo> memList = new List<MemInfo>();

        public MemlockForm()
        {
            InitializeComponent();
            Allocator.setup(System.Diagnostics.Process.GetCurrentProcess());
        }

        private void btnAlloc_Click(object sender, EventArgs e)
        {
            long memSize = (long)(numAllocSize.Value);
            memSize = (memSize + 3) / 4 * 4;
            var ptr = Allocator.alloc(IntPtr.Zero, new IntPtr(memSize * 1024 * 1024));

            if(ptr== IntPtr.Zero)
            {
                checkWin32Error(System.Runtime.InteropServices.Marshal.GetLastWin32Error());
                MessageBox.Show("確保できませんでした");
            }
            else
            {
                var info = new MemInfo();
                info.ptr = ptr;
                info.size = memSize;
                memList.Add(info);
                refreshList();
            }
        }

        private void btnFree_Click(object sender, EventArgs e)
        {
            var idx = lstAllocList.SelectedIndex;
            if (idx >= 0)
            {
                var info = memList[idx];
                
                if(Allocator.free(info.ptr, new IntPtr(info.size * 1024 * 1024)))
                {
                    memList.Remove(info);
                }
                else
                {
                    checkWin32Error(System.Runtime.InteropServices.Marshal.GetLastWin32Error());
                    MessageBox.Show("解放できませんでした");

                }
            }
            refreshList();
        }

        private void refreshList()
        {
            lstAllocList.Enabled = false;
            lstAllocList.Items.Clear();
            foreach (var item in memList)
            {
                lstAllocList.Items.Add(item);
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
    }
}
