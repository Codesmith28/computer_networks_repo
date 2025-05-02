import socket
import struct
import subprocess

# Configuration
MULTICAST_GROUP = input("Multicast address: ")
PORT = int(input("Port: "))


def main():
    # Create a UDP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)

    # Allow multiple sockets to use the same PORT number
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind(("", PORT))

    # Join the multicast group
    mreq = struct.pack("4sL", socket.inet_aton(MULTICAST_GROUP), socket.INADDR_ANY)
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

    # Start VLC
    player_cmd = ["vlc", "-"]
    player_proc = subprocess.Popen(player_cmd, stdin=subprocess.PIPE)

    print(
        f"Listening for multicast stream on {MULTICAST_GROUP}:{PORT} and piping to VLC"
    )

    try:
        while True:
            data, _ = sock.recvfrom(65536)
            if player_proc.poll() is not None:
                print("Player process ended.")
                break
            player_proc.stdin.write(data)
    except KeyboardInterrupt:
        print("Interrupted by user.")
    finally:
        sock.close()
        player_proc.stdin.close()
        player_proc.wait()


if __name__ == "__main__":
    main()
