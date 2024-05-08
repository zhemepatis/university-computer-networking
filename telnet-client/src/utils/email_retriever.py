from email import policy
from email.parser import BytesParser

def get_author(data):
    msg = BytesParser(policy=policy.default).parsebytes(data)
    return msg.get('from', '')


def get_subject(data):
    msg = BytesParser(policy=policy.default).parsebytes(data)
    return msg.get('subject', '')


def get_date(data):
    msg = BytesParser(policy=policy.default).parsebytes(data)
    return msg.get('date', '')


def get_content(data):
    msg = BytesParser(policy=policy.default).parsebytes(data)
    content = ''

    for part in msg.walk():
        if part.get_content_type() == 'text/plain':
            content += part.get_payload(decode=True).decode(part.get_content_charset(), 'ignore')

    return content
