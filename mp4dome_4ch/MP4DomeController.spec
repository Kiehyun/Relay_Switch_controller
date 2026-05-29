# -*- mode: python ; coding: utf-8 -*-
# PyInstaller spec 파일 - MP 4Dome Controller
# 사용법: pyinstaller --noconfirm MP4DomeController.spec

import os
import sys

block_cipher = None


def collect_tree_datas(src_root, dest_root):
    collected = []
    if not os.path.isdir(src_root):
        return collected

    for current_root, _, files in os.walk(src_root):
        for file_name in files:
            source_path = os.path.join(current_root, file_name)
            relative_path = os.path.relpath(current_root, src_root)
            destination_path = dest_root if relative_path == '.' else os.path.join(dest_root, relative_path)
            collected.append((source_path, destination_path))

    return collected


python_root = sys.prefix
tcl_root = os.path.join(python_root, 'Library', 'lib', 'tcl8.6')
tk_root = os.path.join(python_root, 'Library', 'lib', 'tk8.6')
tcl_dll = os.path.join(python_root, 'Library', 'bin', 'tcl86t.dll')
tk_dll = os.path.join(python_root, 'Library', 'bin', 'tk86t.dll')

spec_datas = []
spec_datas.extend(collect_tree_datas(tcl_root, 'tcl8.6'))
spec_datas.extend(collect_tree_datas(tk_root, 'tk8.6'))

spec_binaries = []
if os.path.exists(tcl_dll):
    spec_binaries.append((tcl_dll, '.'))
if os.path.exists(tk_dll):
    spec_binaries.append((tk_dll, '.'))

a = Analysis(
    ['mp4dome_4ch.py'],
    pathex=[os.path.abspath('.')],
    binaries=spec_binaries,
    datas=spec_datas,
    hiddenimports=[
        '_tkinter',
        'serial',
        'serial.tools',
        'serial.tools.list_ports',
        'serial.tools.list_ports_common',
        'serial.tools.list_ports_windows',
        'tkinter',
        'tkinter.ttk',
        'tkinter.messagebox',
        'tweepy',
        'tweepy.auth',
        'tweepy.api',
        'tweepy.models',
    ],
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=[],
    win_no_prefer_redirects=False,
    win_private_assemblies=False,
    cipher=block_cipher,
    noarchive=False,
)

pyz = PYZ(a.pure, a.zipped_data, cipher=block_cipher)

exe = EXE(
    pyz,
    a.scripts,
    a.binaries,
    a.zipfiles,
    a.datas,
    [],
    name='MP4DomeController',
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    upx_exclude=[],
    runtime_tmpdir=None,
    console=False,          # 콘솔창 없음 (GUI 전용)
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
    icon='app.ico' if os.path.exists('app.ico') else None,
)
