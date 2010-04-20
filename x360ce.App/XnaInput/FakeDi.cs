﻿using System;
using System.Collections.Generic;
using System.Text;

namespace x360ce.App.XnaInput
{
        public enum FakeDi
        {
            /// <summary>
            /// Disabled
            /// </summary>
            Disabled = 0,
            /// <summary>
            ///  Enabled: Callback
            /// </summary>
            EnabledCallback = 1,
            /// <summary>
            /// Enabled: Callback + DevInfo
            /// </summary>
            EnabledCallbackAndDevInfo = 2
        }
}