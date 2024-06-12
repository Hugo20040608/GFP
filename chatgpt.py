from openai import OpenAI

client = OpenAI(
    # defaults to os.environ.get("OPENAI_API_KEY")
    api_key="sk-ituckZpetbxX3IAr0x0xXQ1qWK5ntypJJzdodrgRIO1WRSsp",
    base_url="https://api.chatanywhere.tech/v1"
)
# 非流式响应
def gpt_35_api(messages: list):
    
    completion = client.chat.completions.create(model="gpt-3.5-turbo", messages=messages)
    print(completion.choices[0].message.content)

def gpt_35_api_stream(messages: list):
    
    stream = client.chat.completions.create(
        model='gpt-3.5-turbo',
        messages=messages,
        stream=True,
    )
    for chunk in stream:
        if chunk.choices[0].delta.content is not None:
            print(chunk.choices[0].delta.content, end="")

if __name__ == '__main__':
    input_text = input("請輸入對話內容：")
    messages = [{'role': 'user','content': input_text},]
    gpt_35_api_stream(messages)