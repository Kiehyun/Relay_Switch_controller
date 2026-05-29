import os
import sys
import time
import subprocess
import importlib
import shutil
from datetime import datetime
from typing import Optional
import tkinter as tk
from tkinter import messagebox, ttk

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


def _try_install_package(import_name: str, package: str) -> bool:
    attempts = [
        [sys.executable, "-m", "pip", "install", package],
        [sys.executable, "-m", "pip", "install", "--user", package],
    ]

    # pip 자체가 비활성인 환경(일부 conda/임베디드) 대비
    attempts.append([sys.executable, "-m", "ensurepip", "--upgrade"])
    attempts.append([sys.executable, "-m", "pip", "install", package])

    conda_cmd = shutil.which("conda")
    if conda_cmd:
        attempts.append([conda_cmd, "install", "-y", package])

    for cmd in attempts:
        try:
            subprocess.check_call(cmd)
            try:
                importlib.import_module(import_name)
                return True
            except ImportError:
                continue
        except Exception:
            continue

    return False


def _ensure_package(import_name: str, package_name: Optional[str] = None, required: bool = True):
    package = package_name or import_name
    try:
        return importlib.import_module(import_name)
    except ImportError:
        print(f"Missing module '{import_name}'. Installing '{package}'...")
        try:
            installed = _try_install_package(import_name, package)
            if installed:
                return importlib.import_module(import_name)
            raise ImportError(f"Unable to auto-install '{package}'")
        except Exception as exc:
            print(f"Auto-install failed for '{package}': {exc}")
            if required:
                return None
            return None


serial = _ensure_package("serial", "pyserial", required=True)
serial_tools = _ensure_package("serial.tools.list_ports", "pyserial", required=True)
tweepy = _ensure_package("tweepy", "tweepy", required=False)
list_ports = serial_tools


BAUDRATES = [9600]
TIME_SYNC_INTERVAL_MS = 5 * 60 * 1000


def _app_dir() -> str:
    """exe(frozen) 로 실행될 때와 .py 로 실행될 때 모두 올바른 앱 디렉터리를 반환."""
    if getattr(sys, "frozen", False):
        return os.path.dirname(sys.executable)
    return os.path.dirname(os.path.abspath(__file__))


class RelayControllerApp:
    def __init__(self, root: tk.Tk):
        self.dotenv_path = os.path.join(_app_dir(), ".env")
        self.log_path = os.path.join(_app_dir(), "relay_controller.log")
        self.dotenv_loaded = _load_dotenv(self.dotenv_path)

        self.root = root
        self.root.title("MP 4Dome Open/Close Controller")
        self.root.geometry("300x640")
        self.root.configure(bg="black")
        self.root.resizable(False, False)

        self.serial_conn = None
        self.connected_serial = False
        self.previous_second = None
        self.last_action = "-"
        self.last_action_time = "-"
        self.auto_time_sync_enabled = True

        self.bg_disconnected = "#11151c"
        self.bg_connected = "#0f1f1a"
        self.current_bg = self.bg_disconnected

        self.twitter_client = self._build_twitter_client()

        self._build_ui()
        self._log_event("Application started")
        self._show_x_config_status_on_startup()
        self._update_clock_loop()
        self._schedule_time_sync(initial_delay_ms=1000)

    def _build_ui(self):
        style = ttk.Style()
        style.theme_use("clam")

        bg_main = self.current_bg
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

        style.configure(
            "Utility.TButton",
            font=("Segoe UI", 9, "bold"),
            foreground="#ffffff",
            background="#245a8a",
            borderwidth=0,
            padding=4,
        )
        style.map("Utility.TButton", background=[("active", "#2d73b0")])

        tk.Label(
            self.root,
            text="Mang Po High School",
            fg="#f3f6fb",
            bg=bg_main,
            anchor="center",
            font=("Segoe UI", 13, "bold"),
        ).place(x=20, y=18, width=260)

        tk.Label(
            self.root,
            text="4 dome controller",
            fg="#9fb3c8",
            bg=bg_main,
            anchor="center",
            font=("Segoe UI", 15, "bold"),
        ).place(x=20, y=44, width=260)

        ttk.Label(self.root, text="COM Port").place(x=20, y=82)
        self.port_combo = ttk.Combobox(self.root, state="readonly", width=13)
        self.port_combo.place(x=20, y=104)

        ttk.Label(self.root, text="Baudrate").place(x=130, y=82)
        self.baud_combo = ttk.Combobox(self.root, state="readonly", width=8, values=[str(b) for b in BAUDRATES])
        self.baud_combo.place(x=120, y=104)
        self.baud_combo.set("9600")

        self.connect_button = ttk.Button(
            self.root,
            text="Connect",
            style="Connect.TButton",
            command=self.connect_serial,
        )
        self.disconnect_button = ttk.Button(
            self.root,
            text="Disconnect",
            style="Disconnect.TButton",
            command=self.disconnect_serial,
        )
        self._update_connection_buttons()

        self.connection_label = tk.Label(
            self.root,
            text="Serial disconnected",
            fg="#c5d0dd",
            bg=bg_main,
            anchor="w",
            font=("Segoe UI", 9),
        )
        self.connection_label.place(x=20, y=138)

        self.send_time_button = ttk.Button(
            self.root,
            text="PC Time Send",
            style="Utility.TButton",
            command=self.send_pc_time_to_device,
        )
        self.send_time_button.place(x=20, y=162, width=124, height=28)

        self.sync_time_button = ttk.Button(
            self.root,
            text="Time Sync",
            style="Utility.TButton",
            command=self.sync_pc_time_now,
        )
        self.sync_time_button.place(x=156, y=162, width=124, height=28)

        ttk.Button(self.root, text="OPEN", style="ActionOpen.TButton", command=self.do_open).place(
            x=20, y=210, width=260, height=80
        )
        ttk.Button(self.root, text="CLOSE", style="ActionClose.TButton", command=self.do_close).place(
            x=20, y=310, width=260, height=80
        )

        tk.Label(self.root, text="최근 동작 버튼", fg="#d5dee8", bg=bg_main, anchor="w", font=("Segoe UI", 9)).place(x=20, y=420)
        self.last_action_label = tk.Label(
            self.root,
            text="-",
            fg="#53e7a4",
            bg=bg_main,
            anchor="w",
            font=("Segoe UI", 14, "bold"),
        )
        self.last_action_label.place(x=20, y=442)

        tk.Label(self.root, text="최근 동작 시각", fg="#d5dee8", bg=bg_main, anchor="w", font=("Segoe UI", 9)).place(x=20, y=474)
        self.last_time_label = tk.Label(
            self.root,
            text="-",
            fg="#c7d0da",
            bg=bg_main,
            anchor="w",
            font=("Segoe UI", 10),
        )
        self.last_time_label.place(x=20, y=496)

        self.clock_label = tk.Label(
            self.root,
            text="Com Clock",
            fg="#dce6f0",
            bg="#202733",
            anchor="w",
            width=34,
            justify="left",
            font=("Consolas", 9),
        )
        self.clock_label.place(x=20, y=526, width=260, height=34)

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
        self.status_label.place(x=20, y=572)

        self.refresh_ports()

    def _log_event(self, message: str):
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        try:
            with open(self.log_path, "a", encoding="utf-8") as log_file:
                log_file.write(f"[{timestamp}] {message}\n")
        except Exception as exc:
            print(f"Log write failed: {exc}")

    def _update_connection_buttons(self):
        if self.connected_serial:
            self.connect_button.place_forget()
            self.disconnect_button.place(x=185, y=102, width=90, height=24)
        else:
            self.disconnect_button.place_forget()
            self.connect_button.place(x=185, y=102, width=90, height=24)

    def _apply_connection_theme(self, connected: bool):
        self.current_bg = self.bg_connected if connected else self.bg_disconnected
        self.root.configure(bg=self.current_bg)

        # 배경색이 바뀔 때 배경을 공유하는 라벨들도 함께 갱신
        self.connection_label.configure(bg=self.current_bg)
        self.last_action_label.configure(bg=self.current_bg)
        self.last_time_label.configure(bg=self.current_bg)

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
            self._apply_connection_theme(True)
            self._update_connection_buttons()
            self._log_event(f"Serial connected: port={port}, baud={baud}")
            self._set_status("Connected", level="success")
        except Exception as exc:
            self.serial_conn = None
            self.connected_serial = False
            self._apply_connection_theme(False)
            self._update_connection_buttons()
            self._log_event(f"Serial connect failed: port={port}, baud={baud}, error={exc}")
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
            self._apply_connection_theme(False)
            self._update_connection_buttons()
            self._log_event("Serial disconnected")
            self._set_status("Disconnected", level="info")

    def do_open(self):
        if not self._ensure_serial_connected_for_action("열기"):
            return
        if not self._confirm_action("열기"):
            return

        self._send_command_pair("o", "O")
        self._update_last_action("열기(OPEN)")
        self._log_event("Action executed: OPEN")
        self._set_status("OPEN command sent: o, O", level="success")
        self._send_result_to_x("열기")

    def do_close(self):
        if not self._ensure_serial_connected_for_action("닫기"):
            return
        if not self._confirm_action("닫기"):
            return

        self._send_command_pair("c", "C")
        self._update_last_action("닫기(CLOSE)")
        self._log_event("Action executed: CLOSE")
        self._set_status("CLOSE command sent: c, C", level="success")
        self._send_result_to_x("닫기")

    def send_pc_time_to_device(self):
        if not self._ensure_serial_connected_for_action("컴퓨터 시각 전송"):
            return

        payload = "T" + datetime.now().strftime("%H%M%S")
        self._serial_write(payload)
        self._log_event(f"PC time sent to device: payload={payload}")
        self._set_status(f"PC time sent: {payload}", level="success")

    def sync_pc_time_now(self):
        self._log_event("Manual time sync requested")
        self._sync_system_time_with_server(manual=True)

    def _ensure_serial_connected_for_action(self, action_name: str) -> bool:
        if self.connected_serial and self.serial_conn is not None:
            return True

        warning_message = f"시리얼을 먼저 연결한 뒤 {action_name} 버튼을 눌러주세요."
        self._set_status(warning_message, level="warning")
        messagebox.showwarning("시리얼 연결 필요", warning_message)
        return False

    def _confirm_action(self, action_name: str) -> bool:
        confirm_message = f"{action_name} 동작을 실행할까요?"
        confirmed = messagebox.askokcancel("동작 확인", confirm_message)

        if not confirmed:
            self._set_status(f"{action_name} 동작이 취소되었습니다.", level="info")

        return confirmed

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

    def _sync_system_time_with_server(self, manual: bool = False):
        if os.name != "nt":
            print("Time sync skipped: unsupported OS")
            return

        commands = [
            ["w32tm", "/resync"],
            ["w32tm", "/resync", "/force"],
        ]

        last_output = ""
        for command in commands:
            try:
                completed = subprocess.run(
                    command,
                    capture_output=True,
                    text=True,
                    timeout=20,
                    check=False,
                )
                output = (completed.stdout or completed.stderr or "").strip()
                last_output = output
                if completed.returncode == 0:
                    message = f"Time sync OK: {output or 'w32tm /resync succeeded'}"
                    print(message)
                    self._log_event(message)
                    self._set_status("Time sync OK", level="info")
                    return
            except Exception as exc:
                last_output = str(exc)

        raw_error = last_output or "unknown error"
        lower_error = raw_error.lower()
        access_denied = (
            "0x80070005" in lower_error
            or "access is denied" in lower_error
            or "\uc561\uc138\uc2a4\uac00 \uac70\ubd80\ub418\uc5c8\uc2b5\ub2c8\ub2e4" in lower_error
        )

        if access_denied:
            message = f"Time sync failed (admin required): {raw_error}"
            print(message)
            self._log_event(message)
            self._set_status("Time sync failed: 관리자 권한 필요", level="warning")

            if not manual and self.auto_time_sync_enabled:
                self.auto_time_sync_enabled = False
                self._log_event("Auto time sync disabled due to access denied (0x80070005)")
            return

        message = f"Time sync failed: {raw_error}"
        print(message)
        self._log_event(message)
        self._set_status("Time sync failed", level="warning")

    def _schedule_time_sync(self, initial_delay_ms: int = TIME_SYNC_INTERVAL_MS):
        def _run_sync():
            if self.auto_time_sync_enabled:
                self._sync_system_time_with_server()
            self.root.after(TIME_SYNC_INTERVAL_MS, _run_sync)

        self.root.after(initial_delay_ms, _run_sync)

    def _update_clock_loop(self):
        now = datetime.now()
        utc_now = datetime.utcnow()
        self.clock_label.config(
            text=(
                f"KST {now.strftime('%Y-%m-%d %H:%M:%S')}\n"
                f"UT  {utc_now.strftime('%Y-%m-%d %H:%M:%S')}"
            )
        )

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
    if serial is None or list_ports is None:
        missing = []
        if serial is None:
            missing.append("pyserial(import: serial)")
        if list_ports is None:
            missing.append("pyserial(import: serial.tools.list_ports)")

        message = (
            "필수 모듈 자동 설치에 실패했습니다.\n"
            f"누락: {', '.join(missing)}\n\n"
            "아래 명령으로 수동 설치 후 다시 실행해 주세요.\n"
            f"{sys.executable} -m pip install pyserial"
        )

        print(message)
        try:
            temp_root = tk.Tk()
            temp_root.withdraw()
            messagebox.showerror("필수 모듈 설치 실패", message)
            temp_root.destroy()
        except Exception:
            pass
        return

    root = tk.Tk()
    app = RelayControllerApp(root)

    def on_close():
        app.disconnect_serial()
        root.destroy()

    root.protocol("WM_DELETE_WINDOW", on_close)
    root.mainloop()


if __name__ == "__main__":
    main()
