import os
import sys
import time
import subprocess
import importlib
from datetime import datetime
from typing import Optional
import tkinter as tk
from tkinter import ttk

# -------------------------------------------------------
# X(Twitter) .env 설정 방법
# -------------------------------------------------------
# 1) .env.example 을 복사해서 .env 파일 생성
# 2) 아래 4개 값을 .env 에 입력
#    - TWITTER_CONSUMER_KEY
#    - TWITTER_CONSUMER_SECRET
#    - TWITTER_ACCESS_TOKEN
#    - TWITTER_ACCESS_TOKEN_SECRET
# -------------------------------------------------------


def _load_dotenv(dotenv_path: str) -> bool:
    if not os.path.exists(dotenv_path):
        return False

    with open(dotenv_path, "r", encoding="utf-8") as file:
        for raw_line in file:
            line = raw_line.strip()
            if not line or line.startswith("#") or "=" not in line:
                continue

            key, value = line.split("=", 1)
            key = key.strip()
            value = value.strip().strip('"').strip("'")

            if key and key not in os.environ:
                os.environ[key] = value

    return True


def _ensure_package(import_name: str, package_name: Optional[str] = None, required: bool = True):
    package = package_name or import_name
    try:
        return importlib.import_module(import_name)
    except ImportError:
        print(f"Missing module '{import_name}'. Installing '{package}'...")
        try:
            subprocess.check_call([sys.executable, "-m", "pip", "install", package])
            return importlib.import_module(import_name)
        except Exception as exc:
            print(f"Auto-install failed for '{package}': {exc}")
            if required:
                raise
            return None


serial = _ensure_package("serial", "pyserial", required=True)
serial_tools = _ensure_package("serial.tools.list_ports", "pyserial", required=True)
tweepy = _ensure_package("tweepy", "tweepy", required=False)
list_ports = serial_tools


BAUDRATES = [9600]


class RelayControllerApp:
    def __init__(self, root: tk.Tk):
        script_dir = os.path.dirname(os.path.abspath(__file__))
        self.dotenv_path = os.path.join(script_dir, ".env")
        self.dotenv_loaded = _load_dotenv(self.dotenv_path)

        self.root = root
        self.root.title("MP 4Dome Open/Close Controller")
        self.root.geometry("300x500")
        self.root.configure(bg="black")
        self.root.resizable(False, False)

        self.serial_conn = None
        self.connected_serial = False
        self.previous_second = None
        self.last_action = "-"
        self.last_action_time = "-"

        self.twitter_client = self._build_twitter_client()

        self._build_ui()
        self._show_x_config_status_on_startup()
        self._update_clock_loop()

    def _build_ui(self):
        style = ttk.Style()
        style.theme_use("clam")

        bg_main = "#11151c"
        text_main = "#e8ecf1"
        self.root.configure(bg=bg_main)

        style.configure("TLabel", font=("Segoe UI", 9), background=bg_main, foreground=text_main)
        style.configure("TCombobox", font=("Segoe UI", 9), fieldbackground="#f8fafc", background="#d1d9e6")

        style.configure(
            "Connect.TButton",
            font=("Segoe UI", 9, "bold"),
            foreground="#ffffff",
            background="#2f7d32",
            borderwidth=0,
            padding=4,
        )
        style.map("Connect.TButton", background=[("active", "#3f9b43")])

        style.configure(
            "Disconnect.TButton",
            font=("Segoe UI", 9, "bold"),
            foreground="#ffffff",
            background="#8a2432",
            borderwidth=0,
            padding=4,
        )
        style.map("Disconnect.TButton", background=[("active", "#a12d3b")])

        style.configure(
            "ActionOpen.TButton",
            font=("Segoe UI", 16, "bold"),
            foreground="#ffffff",
            background="#1f8b4c",
            borderwidth=0,
            padding=8,
        )
        style.map("ActionOpen.TButton", background=[("active", "#23a559")])

        style.configure(
            "ActionClose.TButton",
            font=("Segoe UI", 16, "bold"),
            foreground="#ffffff",
            background="#b2412d",
            borderwidth=0,
            padding=8,
        )
        style.map("ActionClose.TButton", background=[("active", "#ce4e36")])

        ttk.Label(self.root, text="COM Port").place(x=20, y=36)
        self.port_combo = ttk.Combobox(self.root, state="readonly", width=13)
        self.port_combo.place(x=20, y=58)

        ttk.Label(self.root, text="Baudrate").place(x=130, y=36)
        self.baud_combo = ttk.Combobox(self.root, state="readonly", width=8, values=[str(b) for b in BAUDRATES])
        self.baud_combo.place(x=120, y=58)
        self.baud_combo.set("9600")

        ttk.Button(self.root, text="Connect", style="Connect.TButton", command=self.connect_serial).place(
            x=185, y=56, width=90, height=24
        )
        ttk.Button(self.root, text="Disconnect", style="Disconnect.TButton", command=self.disconnect_serial).place(
            x=185, y=86, width=90, height=24
        )

        self.connection_label = tk.Label(
            self.root,
            text="Serial disconnected",
            fg="#c5d0dd",
            bg=bg_main,
            anchor="w",
            font=("Segoe UI", 9),
        )
        self.connection_label.place(x=20, y=92)

        ttk.Button(self.root, text="OPEN", style="ActionOpen.TButton", command=self.do_open).place(
            x=20, y=140, width=260, height=80
        )
        ttk.Button(self.root, text="CLOSE", style="ActionClose.TButton", command=self.do_close).place(
            x=20, y=240, width=260, height=80
        )

        tk.Label(self.root, text="최근 동작 버튼", fg="#d5dee8", bg=bg_main, anchor="w", font=("Segoe UI", 9)).place(x=20, y=350)
        self.last_action_label = tk.Label(
            self.root,
            text="-",
            fg="#53e7a4",
            bg=bg_main,
            anchor="w",
            font=("Segoe UI", 14, "bold"),
        )
        self.last_action_label.place(x=20, y=372)

        tk.Label(self.root, text="최근 동작 시각", fg="#d5dee8", bg=bg_main, anchor="w", font=("Segoe UI", 9)).place(x=20, y=404)
        self.last_time_label = tk.Label(
            self.root,
            text="-",
            fg="#c7d0da",
            bg=bg_main,
            anchor="w",
            font=("Segoe UI", 10),
        )
        self.last_time_label.place(x=20, y=426)

        self.clock_label = tk.Label(
            self.root,
            text="Com Clock",
            fg="#dce6f0",
            bg="#202733",
            anchor="w",
            width=34,
            font=("Consolas", 10),
        )
        self.clock_label.place(x=20, y=452)

        self.status_label = tk.Label(
            self.root,
            text="Ready",
            fg="#0b1524",
            bg="#d9ecff",
            anchor="w",
            width=34,
            font=("Segoe UI", 9, "bold"),
            padx=8,
        )
        self.status_label.place(x=20, y=476)

        self.refresh_ports()

    def refresh_ports(self):
        ports = [p.device for p in list_ports.comports()]
        self.port_combo["values"] = ports
        if ports and not self.port_combo.get():
            self.port_combo.current(0)

    def connect_serial(self):
        if self.connected_serial:
            self._set_status("Already connected to a port!", level="warning")
            return

        port = self.port_combo.get().strip()
        baud = self.baud_combo.get().strip()

        if not port:
            self._set_status("Select COM Port first!", level="warning")
            return
        if not baud:
            self._set_status("Select baudrate first!", level="warning")
            return

        try:
            self.serial_conn = serial.Serial(port, int(baud), timeout=0.2)
            self.connected_serial = True
            self._serial_write("G;")
            self.connection_label.config(text="Serial connected")
            self._set_status("Connected", level="success")
        except Exception as exc:
            self.serial_conn = None
            self.connected_serial = False
            self._set_status(f"Connect failed: {exc}", level="error")

    def disconnect_serial(self):
        if not self.connected_serial:
            self._set_status("Couldn't disconnect", level="warning")
            return

        try:
            if self.serial_conn is not None:
                self.serial_conn.close()
        finally:
            self.serial_conn = None
            self.connected_serial = False
            self.connection_label.config(text="Serial disconnected")
            self._set_status("Disconnected", level="info")

    def do_open(self):
        self._send_command_pair("o", "O")
        self._update_last_action("열기(OPEN)")
        self._set_status("OPEN command sent: o, O", level="success")
        self._send_result_to_x("열기")

    def do_close(self):
        self._send_command_pair("c", "C")
        self._update_last_action("닫기(CLOSE)")
        self._set_status("CLOSE command sent: c, C", level="success")
        self._send_result_to_x("닫기")

    def _send_command_pair(self, cmd1: str, cmd2: str):
        self._serial_write(cmd1)
        time.sleep(0.02)
        self._serial_write(cmd2)
        time.sleep(0.02)

    def _update_last_action(self, action_name: str):
        now = datetime.now()
        self.last_action = action_name
        self.last_action_time = now.strftime("%Y-%m-%d %H:%M:%S")
        self.last_action_label.config(text=self.last_action)
        self.last_time_label.config(text=self.last_action_time)

    def _serial_write(self, payload: str):
        if not self.connected_serial or self.serial_conn is None:
            return
        self.serial_conn.write(payload.encode("ascii", errors="ignore"))

    def _set_status(self, text: str, level: str = "info"):
        color_map = {
            "success": {"fg": "#003300", "bg": "#9cff9c"},
            "warning": {"fg": "#4a3400", "bg": "#ffd27a"},
            "error": {"fg": "#5a0000", "bg": "#ff9d9d"},
            "info": {"fg": "#000000", "bg": "#d9ecff"},
        }
        palette = color_map.get(level, color_map["info"])
        self.status_label.config(text=text, fg=palette["fg"], bg=palette["bg"])
        print(text)

    def _missing_x_keys(self):
        required = [
            "TWITTER_CONSUMER_KEY",
            "TWITTER_CONSUMER_SECRET",
            "TWITTER_ACCESS_TOKEN",
            "TWITTER_ACCESS_TOKEN_SECRET",
        ]
        return [key for key in required if not os.getenv(key)]

    def _show_x_config_status_on_startup(self):
        missing_keys = self._missing_x_keys()

        if not self.dotenv_loaded:
            self._set_status(".env 파일 없음: X 전송 비활성화", level="warning")
            return

        if missing_keys:
            self._set_status(f".env 키 누락: {', '.join(missing_keys)}", level="warning")
            return

        self._set_status("X 설정 완료", level="success")

    def _update_clock_loop(self):
        now = datetime.now()
        self.clock_label.config(text=f"Com Clock        {now.hour} : {now.minute} : {now.second}")

        if self.connected_serial:
            if self.previous_second != now.second and now.second in (0, 15, 30, 45):
                print(now.strftime("%Y%m%d%H%M%S"))

        self.previous_second = now.second
        self.root.after(200, self._update_clock_loop)

    def _build_twitter_client(self):
        if tweepy is None:
            return None

        consumer_key = os.getenv("TWITTER_CONSUMER_KEY")
        consumer_secret = os.getenv("TWITTER_CONSUMER_SECRET")
        access_token = os.getenv("TWITTER_ACCESS_TOKEN")
        access_token_secret = os.getenv("TWITTER_ACCESS_TOKEN_SECRET")

        if not all([consumer_key, consumer_secret, access_token, access_token_secret]):
            return None

        try:
            auth = tweepy.OAuth1UserHandler(
                consumer_key,
                consumer_secret,
                access_token,
                access_token_secret,
            )
            return tweepy.API(auth)
        except Exception as exc:
            print(f"Twitter client init failed: {exc}")
            return None

    def _twitter_send(self, message: str):
        if self.twitter_client is None:
            return

        try:
            tweet_text = f"{message}{datetime.now().strftime('%Y%m%d%H%M%S')}"
            self.twitter_client.update_status(tweet_text)
            print(f"Status updated to [{tweet_text}].")
        except Exception as exc:
            print(f"Error: {exc}")

    def _send_result_to_x(self, action_name: str):
        if self.twitter_client is None:
            self._set_status(f"{action_name} 동작 완료 (X 미설정)", level="warning")
            return

        text = f"[돔 제어] {action_name} 동작 실행 / 시각: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}"
        self._twitter_send(text + " ")


def main():
    root = tk.Tk()
    app = RelayControllerApp(root)

    def on_close():
        app.disconnect_serial()
        root.destroy()

    root.protocol("WM_DELETE_WINDOW", on_close)
    root.mainloop()


if __name__ == "__main__":
    main()
