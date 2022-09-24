namespace memlock
{
    partial class MemlockForm
    {
        /// <summary>
        /// 必要なデザイナー変数です。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 使用中のリソースをすべてクリーンアップします。
        /// </summary>
        /// <param name="disposing">マネージド リソースを破棄する場合は true を指定し、その他の場合は false を指定します。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows フォーム デザイナーで生成されたコード

        /// <summary>
        /// デザイナー サポートに必要なメソッドです。このメソッドの内容を
        /// コード エディターで変更しないでください。
        /// </summary>
        private void InitializeComponent()
        {
            this.numAllocSize = new System.Windows.Forms.NumericUpDown();
            this.lstAllocList = new System.Windows.Forms.ListBox();
            this.btnAlloc = new System.Windows.Forms.Button();
            this.btnFree = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.numAllocCount = new System.Windows.Forms.NumericUpDown();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.lblTotalSize = new System.Windows.Forms.Label();
            this.cboMemDevice = new System.Windows.Forms.ComboBox();
            ((System.ComponentModel.ISupportInitialize)(this.numAllocSize)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numAllocCount)).BeginInit();
            this.SuspendLayout();
            // 
            // numAllocSize
            // 
            this.numAllocSize.Location = new System.Drawing.Point(20, 18);
            this.numAllocSize.Margin = new System.Windows.Forms.Padding(5, 4, 5, 4);
            this.numAllocSize.Maximum = new decimal(new int[] {
            1024,
            0,
            0,
            0});
            this.numAllocSize.Minimum = new decimal(new int[] {
            4,
            0,
            0,
            0});
            this.numAllocSize.Name = "numAllocSize";
            this.numAllocSize.Size = new System.Drawing.Size(118, 25);
            this.numAllocSize.TabIndex = 1;
            this.numAllocSize.Value = new decimal(new int[] {
            256,
            0,
            0,
            0});
            this.numAllocSize.ValueChanged += new System.EventHandler(this.numAllocSize_ValueChanged);
            // 
            // lstAllocList
            // 
            this.lstAllocList.FormattingEnabled = true;
            this.lstAllocList.ItemHeight = 18;
            this.lstAllocList.Location = new System.Drawing.Point(22, 93);
            this.lstAllocList.Margin = new System.Windows.Forms.Padding(5, 4, 5, 4);
            this.lstAllocList.Name = "lstAllocList";
            this.lstAllocList.Size = new System.Drawing.Size(577, 256);
            this.lstAllocList.TabIndex = 2;
            // 
            // btnAlloc
            // 
            this.btnAlloc.Location = new System.Drawing.Point(213, 14);
            this.btnAlloc.Margin = new System.Windows.Forms.Padding(5, 4, 5, 4);
            this.btnAlloc.Name = "btnAlloc";
            this.btnAlloc.Size = new System.Drawing.Size(85, 64);
            this.btnAlloc.TabIndex = 3;
            this.btnAlloc.Text = "alloc";
            this.btnAlloc.UseVisualStyleBackColor = true;
            this.btnAlloc.Click += new System.EventHandler(this.btnAlloc_Click);
            // 
            // btnFree
            // 
            this.btnFree.Location = new System.Drawing.Point(20, 362);
            this.btnFree.Margin = new System.Windows.Forms.Padding(5, 4, 5, 4);
            this.btnFree.Name = "btnFree";
            this.btnFree.Size = new System.Drawing.Size(582, 34);
            this.btnFree.TabIndex = 4;
            this.btnFree.Text = "free";
            this.btnFree.UseVisualStyleBackColor = true;
            this.btnFree.Click += new System.EventHandler(this.btnFree_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(148, 21);
            this.label1.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(32, 18);
            this.label1.TabIndex = 5;
            this.label1.Text = "MB";
            // 
            // numAllocCount
            // 
            this.numAllocCount.Location = new System.Drawing.Point(20, 57);
            this.numAllocCount.Margin = new System.Windows.Forms.Padding(5, 4, 5, 4);
            this.numAllocCount.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numAllocCount.Name = "numAllocCount";
            this.numAllocCount.Size = new System.Drawing.Size(118, 25);
            this.numAllocCount.TabIndex = 6;
            this.numAllocCount.Value = new decimal(new int[] {
            8,
            0,
            0,
            0});
            this.numAllocCount.ValueChanged += new System.EventHandler(this.numAllocCount_ValueChanged);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(148, 60);
            this.label2.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(50, 18);
            this.label2.TabIndex = 5;
            this.label2.Text = "count";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(310, 58);
            this.label3.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(140, 18);
            this.label3.TabIndex = 7;
            this.label3.Text = "total allocate size";
            // 
            // lblTotalSize
            // 
            this.lblTotalSize.AutoSize = true;
            this.lblTotalSize.Location = new System.Drawing.Point(482, 57);
            this.lblTotalSize.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.lblTotalSize.Name = "lblTotalSize";
            this.lblTotalSize.Size = new System.Drawing.Size(40, 18);
            this.lblTotalSize.TabIndex = 8;
            this.lblTotalSize.Text = "2GB";
            // 
            // cboMemDevice
            // 
            this.cboMemDevice.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboMemDevice.FormattingEnabled = true;
            this.cboMemDevice.Items.AddRange(new object[] {
            "System"});
            this.cboMemDevice.Location = new System.Drawing.Point(313, 13);
            this.cboMemDevice.Name = "cboMemDevice";
            this.cboMemDevice.Size = new System.Drawing.Size(335, 26);
            this.cboMemDevice.TabIndex = 9;
            // 
            // MemlockForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(10F, 18F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(660, 435);
            this.Controls.Add(this.cboMemDevice);
            this.Controls.Add(this.lblTotalSize);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.numAllocCount);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.btnFree);
            this.Controls.Add(this.btnAlloc);
            this.Controls.Add(this.lstAllocList);
            this.Controls.Add(this.numAllocSize);
            this.Margin = new System.Windows.Forms.Padding(5, 4, 5, 4);
            this.Name = "MemlockForm";
            this.Text = "memlock";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.MemlockForm_FormClosed);
            ((System.ComponentModel.ISupportInitialize)(this.numAllocSize)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numAllocCount)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.NumericUpDown numAllocSize;
        private System.Windows.Forms.ListBox lstAllocList;
        private System.Windows.Forms.Button btnAlloc;
        private System.Windows.Forms.Button btnFree;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.NumericUpDown numAllocCount;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label lblTotalSize;
        private System.Windows.Forms.ComboBox cboMemDevice;
    }
}

